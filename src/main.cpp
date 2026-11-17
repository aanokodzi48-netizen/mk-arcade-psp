#include <SDL.h>
#include <pspkernel.h>
#include <stdio.h>

PSP_MODULE_INFO("MK PSP TEST", 0, 1, 1);

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL init failed: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Surface *screen = SDL_SetVideoMode(480, 272, 16, SDL_SWSURFACE);
    if (!screen) {
        printf("Video mode failed\n");
        SDL_Quit();
        return 1;
    }
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
        }
        SDL_FillRect(screen, NULL, SDL_MapRGB(screen->format, 0, 0, 0));
        SDL_Flip(screen);
    }
    SDL_Quit();
    return 0;
}
