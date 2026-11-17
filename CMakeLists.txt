cmake_minimum_required(VERSION 3.10)
project(mkpsp CXX)
add_executable(mkpsp src/main.cpp)
target_link_libraries(mkpsp SDL SDL_image SDL_ttf SDL_mixer pspdisplay pspctrl pspdebug pspkernel)

find_program(MKSFO mksfo REQUIRED)
find_program(PACKPBP pack-pbp REQUIRED)

add_custom_command(TARGET mkpsp POST_BUILD
  COMMAND ${MKSFO} "MK PSP Test" PARAM.SFO
  COMMAND ${PACKPBP} EBOOT.PBP PARAM.SFO NULL NULL NULL NULL NULL $<TARGET_FILE:mkpsp> NULL
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Packaging EBOOT.PBP")
