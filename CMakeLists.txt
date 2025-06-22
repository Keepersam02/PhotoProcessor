cmake_minimum_required(VERSION 3.30)
project(PhotoProcessor C)

set(CMAKE_C_STANDARD 23)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LibRaw REQUIRED libraw)

include_directories(${LibRaw_INCLUDE_DIRS})

add_executable(PhotoProcessor main.c
        file_handling/import.c
        file_handling/import.h
        file_handling/bmpWrite.c
        file_handling/bmpWrite.h
)

target_link_libraries(PhotoProcessor ${LibRaw_LIBRARIES})