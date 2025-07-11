cmake_minimum_required(VERSION 3.30)
project(PhotoProcessor C)

set(CMAKE_C_STANDARD 23)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LibTiff REQUIRED libtiff-4)
pkg_check_modules(LibRaw REQUIRED libraw)

include_directories(${LibRaw_INCLUDE_DIRS})
include_directories(${LibTiff_INCLUDE_DIRS})

add_executable(PhotoProcessor main.c
        file_handling/import.c
        file_handling/import.h
        file_handling/bmpWrite.c
        file_handling/bmpWrite.h
        lut-handle/lutApp.c
        lut-handle/lutApp.h
        lut-handle/lutParse.c
        lut-handle/lutParse.h
        data_structures/imageVector.c
        data_structures/imageVector.h
        color_management/CSTApp.c
        color_management/CSTApp.h
        file_handling/writeTiff.c
)

target_link_libraries(PhotoProcessor ${LibRaw_LIBRARIES} ${LibTiff_LIBRARIES})