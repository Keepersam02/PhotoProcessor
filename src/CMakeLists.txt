add_library(bpp_lib
  io/image_ver.cpp
  io/image_ver.hpp
  io/image_io_error.hpp
  io/image_io.cpp
  io/image_io.hpp
  image_handling/image_handler.cpp
  image_handling/image_handler.hpp
)


target_compile_options(bpp_lib PRIVATE
  $<$<CONFIG:Debug>:
    -Wall;
    -Wextra;
    -Werror;
    -g;
    -O0;
    -Wpedantic;
    -Wshadow;
    -Wnon-virtual-dtor;
    -Wold-style-cast;
    -Wcast-align;
    -Wunused;
    -Woverloaded-virtual;
    -Wconversion;
    -Wsign-conversion;
    >
)

target_compile_features(bpp_lib PUBLIC cxx_std_23)

target_compile_options(bpp_lib PRIVATE
  $<$<CONFIG:Release>:-O3>
)

target_include_directories(bpp_lib PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(bpp_lib SYSTEM PRIVATE /usr/include/tiff)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")

  target_compile_options(bpp_lib PUBLIC
  -fsanitize=address;
  )
  target_compile_options(bpp_lib PUBLIC -fno-omit-frame-pointer)
  target_link_options(bpp_lib PUBLIC -fsanitize=address)
endif()

include(FetchContent)
# Fetch zlib first (dependency for libpng and libtiff)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG        v1.3.1  # Or whatever version you need
)
FetchContent_MakeAvailable(zlib)

# Fetch libpng (depends on zlib)
FetchContent_Declare(
  libpng
  URL http://prdownloads.sourceforge.net/libpng/lpng1653.zip
)
set(PNG_DEBUG_POSTFIX "" CACHE STRING "Disable debug suffix" FORCE)
FetchContent_MakeAvailable(libpng)

# Fetch libtiff
FetchContent_Declare(
  libtiff
  URL https://gitlab.com/libtiff/libtiff/-/archive/v4.7.1/libtiff-v4.7.1.zip
)
FetchContent_MakeAvailable(libtiff)

find_package(exiv2 REQUIRED)

# Find system packages for JPEG and LibRaw
find_package(JPEG REQUIRED)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LibRaw REQUIRED libraw_r)
pkg_check_modules(Exiv2 REQUIRED exiv2)
target_include_directories(bpp_lib PRIVATE ${LibRaw_INCLUDE_DIRS}
${Exiv2_INCLUDE_DIRS}
)

# Link libraries - use PUBLIC to propagate include directories
target_link_libraries(bpp_lib PRIVATE
    JPEG::JPEG      # libjpeg (system)
    ${LibRaw_LIBRARIES}
    ${Exiv2_LIBRARIES} # LibRaw
)

# Add png library with proper target
target_link_libraries(bpp_lib PRIVATE png_static)

# Add tiff library
target_link_libraries(bpp_lib PRIVATE tiff)

# Add zlib
target_link_libraries(bpp_lib PRIVATE zlibstatic)
