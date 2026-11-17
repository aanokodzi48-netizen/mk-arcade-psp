#include <pspkernel.h>
#include <pspdebug.h>
#include <pspctrl.h>
#include <pspdisplay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PSP_MODULE_INFO("MK PSP", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

static int exitCb(int a1, int a2, void *c) { sceKernelExitGame(); return 0; }
static int cbThread(SceSize args, void *argp) {
    int cb = sceKernelCreateCallback("exit", exitCb, NULL);
    sceKernelRegisterExitCallback(cb);
    sceKernelSleepThreadCB();
    return 0;
}
static void setupExit(void) {
    int th = sceKernelCreateThread("exit", cbThread, 0x11, 0xFA0, 0, NULL);
    if (th >= 0) sceKernelStartThread(th, 0, NULL);
}

#define AW 40
#define AH 14
static char grid[AH][AW + 1];
static int clamp(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

int main(void) {
    setupExit();
    pspDebugScreenInit();
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    int p1x = 8, p1y = 7, p2x = 30, p2y = 7;
    int hp1 = 100, hp2 = 100, frame = 0, msgT = -999;
    char msg[40] = "";
    int hist[3] = {0, 0, 0}, histT[3] = {-999, -999, -999};
    unsigned int prev = 0;
    SceCtrlData pad;

    while (1) {
        sceCtrlReadBufferPositive(&pad, 1);
        if (pad.Buttons & PSP_CTRL_START) break;

        if (pad.Buttons & PSP_CTRL_LEFT)  p1x--;
        if (pad.Buttons & PSP_CTRL_RIGHT) p1x++;
        if (pad.Buttons & PSP_CTRL_UP)    p1y--;
        if (pad.Buttons & PSP_CTRL_DOWN)  p1y++;
        p1x = clamp(p1x, 0, AW - 1); p1y = clamp(p1y, 0, AH - 1);

        unsigned int press = pad.Buttons & ~prev; prev = pad.Buttons;
        int act = 0;
        if (press & PSP_CTRL_DOWN)  act = 1;
        if (press & PSP_CTRL_RIGHT) act = 2;
        if (press & PSP_CTRL_CROSS) act = 3;
        if (press & PSP_CTRL_CIRCLE) act = 4;
        int block = (pad.Buttons & PSP_CTRL_SQUARE) ? 1 : 0;
        if (act) {
            hist[0] = hist[1]; histT[0] = histT[1];
            hist[1] = hist[2]; histT[1] = histT[2];
            hist[2] = act;     histT[2] = frame;
        }
        int special = (hist[2] == 3 && hist[1] == 2 && hist[0] == 1 && frame - histT[0] < 45);
        int near = (abs(p1x - p2x) <= 2 && abs(p1y - p2y) <= 1);

        if (act == 3 || act == 4) {
            if (near) {
                int d = special ? 25 : (act == 3 ? 8 : 10);
                hp2 -= d;
                strcpy(msg, special ? "SPECIAL! freeze hit" : (act == 3 ? "PUNCH lands" : "KICK lands"));
                msgT = frame;
                if (special) hist[0] = 0;
            } else { strcpy(msg, "whiff (out of range)"); msgT = frame; }
        }
        if (frame % 20 == 0) { if (p2x > p1x + 2) p2x--; else if (p2x < p1x - 2) p2x++; }
        if (frame % 50 == 0 && near) { hp1 -= block ? 2 : 12; strcpy(msg, block ? "P2 hit BLOCKED" : "P2 hits you"); msgT = frame; }

        pspDebugScreenClear();
        pspDebugScreenSetXY(0, 0);
        pspDebugScreenPrintf("P1 HP %3d | P2 HP %3d | f%d\n", hp1, hp2, frame);
        pspDebugScreenPrintf("%s\n", (frame - msgT < 40) ? msg : "");
        for (int y = 0; y < AH; y++) { memset(grid[y], ' ', AW); grid[y][AW] = 0; }
        grid[p2y][p2x] = '2';
        grid[p1y][p1x] = block ? 'B' : '1';
        if (near && (act == 3 || act == 4)) grid[p1y][clamp(p1x + 1, 0, AW - 1)] = '*';
        for (int y = 0; y < AH; y++) pspDebugScreenPrintf("%s\n", grid[y]);
        pspDebugScreenPrintf("dpad=move X=punch O=kick []=block START=quit\n");
        pspDebugScreenPrintf("combo: DOWN,FWD,X = special\n");
        sceDisplayWaitVblankStart();
        frame++;
        if (hp1 <= 0 || hp2 <= 0) {
            pspDebugScreenPrintf("\n%s WINS\n", hp2 <= 0 ? "P1" : "P2");
            sceDisplayWaitVblankStart();
            for (int i = 0; i < 120; i++) sceDisplayWaitVblankStart();
            break;
        }
    }
    sceKernelExitGame();
    return 0;
}
