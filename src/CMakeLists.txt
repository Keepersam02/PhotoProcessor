add_library(bpp_lib
  io/in.cpp
  io/in.hpp
  proc/proc.cpp
  proc/proc.hpp
  types/file_slab.hpp
  types/misc.hpp
  bpp_loop.cpp
  bpp_loop.hpp
  db/db_manag.cpp
  db/db_manag.hpp
)

target_compile_options(bpp_lib PRIVATE
  $<$<CONFIG:Debug>:
  -Wall;
  -Wextra;
  -Wpedantic;
  -g;
  -Wshadow;
  -Wunused;
  >
)

target_include_directories(bpp_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(SQLite3 REQUIRED)

find_package(PkgConfig REQUIRED)

pkg_check_modules(LibRaw REQUIRED libraw_r)
target_include_directories(bpp_lib PRIVATE ${LibRaw_INCLUDE_DIRS})

target_link_libraries(bpp_lib PRIVATE ${LibRaw_LIBRARIES} SQLite3::SQLite3)
