add_library(bpp_lib
  io/in.cpp
  io/in.hpp
)

target_compile_options(bpp_lib PRIVATE
  $<-Wall
  -Wextra
  -Wpedantic
  -g
  -Wshadow
  -Wunused
  >
)

find_package(PkgConfig REQUIRED)

pkg_check_modules(LibRaw REQUIRED libraw_r)
target_include_directories(bpp_lib PRIVATE ${LibRaw_INCLUDE_DIRS})

target_link_libraries(bpp_lib PRIVATE ${LibRaw_LIBRARIES})
