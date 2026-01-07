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

# Fetch zlib first (dependency for libpng and libtiff)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
FetchContent_Declare(
  zlib
  URL https://zlib.net/zlib131.zip
)
FetchContent_MakeAvailable(zlib)

# Fetch libpng (depends on zlib)
FetchContent_Declare(
  libpng
  URL http://prdownloads.sourceforge.net/libpng/lpng1653.zip
)
FetchContent_MakeAvailable(libpng)

# Fetch libtiff
FetchContent_Declare(
  libtiff
  URL https://gitlab.com/libtiff/libtiff/-/archive/v4.7.1/libtiff-v4.7.1.zip
)
FetchContent_MakeAvailable(libtiff)

# Fetch Exiv2
FetchContent_Declare(
  exiv2
  URL https://github.com/Exiv2/exiv2/archive/refs/tags/v0.28.7.zip
)
FetchContent_MakeAvailable(exiv2)

# Find system packages for JPEG and LibRaw
find_package(JPEG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LibRaw REQUIRED libraw_r)
target_include_directories(bpp_lib PRIVATE ${LibRaw_INCLUDE_DIRS})

# Link libraries - use PUBLIC to propagate include directories
target_link_libraries(bpp_lib PRIVATE
    JPEG::JPEG      # libjpeg (system)
    ${LibRaw_LIBRARIES}  # LibRaw
    exiv2lib        # Exiv2
)

# Add png library with proper target
target_link_libraries(bpp_lib PRIVATE png_static)

# Add tiff library
target_link_libraries(bpp_lib PRIVATE tiff)

# Add zlib
target_link_libraries(bpp_lib PRIVATE zlibstatic)
