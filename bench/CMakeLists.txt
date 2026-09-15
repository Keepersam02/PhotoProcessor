add_executable(io_bench_std_t io/io_benchmark_std.cpp)
add_executable(io_bench_std_r io/io_benchmark_std.cpp)

target_compile_features(io_bench_std_t PUBLIC cxx_std_23)
target_compile_features(io_bench_std_r PUBLIC cxx_std_23)

target_compile_options(io_bench_std_t PRIVATE -O1 -g -fno-omit-frame-pointer -Wall -Wextra -Werror)
target_compile_options(io_bench_std_r PRIVATE -O3 -g -fno-omit-frame-pointer -Wall -Wextra -Werror)

target_link_libraries(io_bench_std_t PRIVATE bpp_lib)
target_link_libraries(io_bench_std_r PRIVATE bpp_lib)
