#main
CMAKE_MINIMUM_REQUIRED(VERSION 3.28)
project(batch_photo_processor)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for vscode
set(CMAKE_BUILD_TYPE Debug) # for gdb

add_subdirectory(src)
add_subdirectory(app)
# add_subdirectory(test)

