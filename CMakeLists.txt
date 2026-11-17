cmake_minimum_required(VERSION 3.10)

set(CMAKE_C_COMPILER psp-gcc)
set(CMAKE_CXX_COMPILER psp-g++)

# Указываем пути к PSP SDK
set(PSPSDK $ENV{PSP_SDK})
if(NOT PSPSDK)
    set(PSPSDK /usr/local/pspdev/psp/sdk)
endif()

include_directories(${PSPSDK}/include)
link_directories(${PSPSDK}/lib)

project(mkpsp CXX)
add_executable(mkpsp src/main.cpp)
target_link_libraries(mkpsp pspdebug pspdisplay pspctrl pspkernel)

find_program(MKSFO mksfo REQUIRED)
find_program(PACKPBP pack-pbp REQUIRED)

add_custom_command(TARGET mkpsp POST_BUILD
  COMMAND ${MKSFO} "MK PSP" PARAM.SFO
  COMMAND ${PACKPBP} EBOOT.PBP PARAM.SFO NULL NULL NULL NULL NULL $<TARGET_FILE:mkpsp> NULL
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Packaging EBOOT.PBP")
