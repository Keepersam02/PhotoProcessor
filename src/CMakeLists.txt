add_library(bpp_lib
  temp.cpp
  io/image_io.cpp
  io/image_io.hpp
)

target_compile_options(bpp_lib PRIVATE
  $<$<CONFIG:Debug>:-Wall -Wextra -Werror -Og>
)

target_compile_options(bpp_lib PRIVATE
  $<$<CONFIG:Release>:-O3>
)

target_include_directories(bpp_lib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

include(FetchContent)
FetchContent_Declare(
  exiv2
  URL https://github.com/Exiv2/exiv2/archive/refs/tags/v0.28.7.zip
)
FetchContent_MakeAvailable(exiv2)

FetchContent_Declare(
  libtiff
  URL https://gitlab.com/libtiff/libtiff/-/archive/v4.7.1/libtiff-v4.7.1.zip
)
FetchContent_MakeAvailable(libtiff)

find_package(JPEG REQUIRED)

FetchContent_Declare(
  zlib
  URL https://zlib.net/zlib131.zip
)
FetchContent_MakeAvailable(zlib)

FetchContent_Declare(
  libpng
  URL http://prdownloads.sourceforge.net/libpng/lpng1653.zip
)
FetchContent_MakeAvailable(libpng)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LibRaw REQUIRED libraw_r)
target_include_directories(bpp_lib PRIVATE ${LibRaw_INCLUDE_DIRS})

target_link_libraries(bpp_lib PRIVATE
  ${LibRaw_LIBRARIES}
  exiv2lib
)

