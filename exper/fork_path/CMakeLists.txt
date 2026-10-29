
add_executable(fork_exper
  fork.cpp
)

target_compile_options(fork_exper PRIVATE -Wall -g)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LibRaw REQUIRED libraw_r)
target_include_directories(fork_exper PRIVATE ${LibRaw_INCLUDE_DIRS})
target_link_libraries(fork_exper PRIVATE
 ${LibRaw_LIBRARIES}

)
