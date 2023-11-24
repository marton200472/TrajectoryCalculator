cmake_minimum_required(VERSION 3.26)
project(NHF C)

set(CMAKE_C_STANDARD 11)

add_executable(NHF main.c
        environment.c
        environment.h
        noise.c
        noise.h
        2darr.c
        2darr.h
        trajectory.c
        trajectory.h
        file.c
        file.h
        artydata.c
        artydata.h
        vector2.h
        colortools.c
        colortools.h
        minmax.c
        minmax.h
        parameter.c
        parameter.h
)

INCLUDE(FindPkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
PKG_SEARCH_MODULE(SDL2GFX REQUIRED SDL2_gfx)
PKG_SEARCH_MODULE(SDL2TTF REQUIRED SDL2_ttf)
PKG_SEARCH_MODULE(SDL2IMAGE REQUIRED SDL2_image)

INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIRS} ${SDL2GFX_INCLUDE_DIRS} ${SDL2TTF_INCLUDE_DIRS} ${SDL2IMAGE_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2GFX_LIBRARIES} ${SDL2TTF_LIBRARIES} ${SDL2IMAGE_LIBRARIES})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} m pthread)

target_compile_options(NHF PRIVATE -Wall -Werror)