add_executable(fork_exp fork.cpp)

target_compile_options(fork_exp PRIVATE -Wall -Wextra -g)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LibRaw REQUIRED libraw_r)
target_include_directories(fork_exp PRIVATE ${LibRaw_INCLUDE_DIRS}
)

target_link_libraries(fork_exp PRIVATE
    ${LibRaw_LIBRARIES}
)
