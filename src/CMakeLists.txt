#src
add_library(bpp_lib
  io/image_io.cpp
  io/image_io.hpp
  db/database.cpp
  db/database.hpp
  db/schema.cpp
  db/schema.hpp
  db/models/batch.hpp
  db/repos/batch_repo.cpp
  db/repos/batch_repo.hpp
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
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG        v1.3.1
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
  GIT_REPOSITORY https://gitlab.com/libtiff/libtiff.git
  GIT_TAG        v4.7.0
)
FetchContent_MakeAvailable(libtiff)

#Fetch SQLite
FetchContent_Declare(
  SQLiteCpp
  GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
  GIT_TAG        3.3.1
)
FetchContent_MakeAvailable(SQLiteCpp)

# Fetch Exiv2
FetchContent_Declare(
  exiv2
  GIT_REPOSITORY https://github.com/Exiv2/exiv2.git
  GIT_TAG        v0.28.7
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

# Add SQLiteCpp
target_link_libraries(bpp_lib PUBLIC SQLiteCpp sqlite3)