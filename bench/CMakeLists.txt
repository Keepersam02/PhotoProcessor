add_executable(io_bench_std_o1 io/io_benchmark_std.cpp)
add_executable(io_bench_std_o3 io/io_benchmark_std.cpp)
add_executable(io_bench_sys_o1 io/io_benchmark_std.cpp)
add_executable(io_bench_sys_o3 io/io_benchmark_std.cpp)

target_compile_features(io_bench_std_o1 PUBLIC cxx_std_23)
target_compile_features(io_bench_std_o3 PUBLIC cxx_std_23)
target_compile_features(io_bench_sys_o1 PUBLIC cxx_std_23)
target_compile_features(io_bench_sys_o3 PUBLIC cxx_std_23)

target_compile_options(io_bench_std_o1 PRIVATE -O1 -g -fno-omit-frame-pointer -Wall -Wextra -Werror)
target_compile_options(io_bench_std_o3 PRIVATE -O3 -g -fno-omit-frame-pointer -Wall -Wextra -Werror)
target_compile_options(io_bench_sys_o1 PRIVATE -O1 -g -fno-omit-frame-pointer -Wall -Wextra -Werror)
target_compile_options(io_bench_sys_o3 PRIVATE -O3 -g -fno-omit-frame-pointer -Wall -Wextra -Werror)

target_link_libraries(io_bench_std_o1 PRIVATE bpp_lib)
target_link_libraries(io_bench_std_o3 PRIVATE bpp_lib)
target_link_libraries(io_bench_sys_o1 PRIVATE bpp_lib)
target_link_libraries(io_bench_sys_o3 PRIVATE bpp_lib)

target_compile_definitions(io_bench_std_o1 PRIVATE TEST_IMAGE_DIR="${CMAKE_PROJECT_SOURCE_DIR}")
target_compile_definitions(io_bench_std_o3 PRIVATE TEST_IMAGE_DIR="${CMAKE_PROJECT_SOURCE_DIR}")
target_compile_definitions(io_bench_sys_o1 PRIVATE TEST_IMAGE_DIR="${CMAKE_PROJECT_SOURCE_DIR}")
target_compile_definitions(io_bench_sys_o3 PRIVATE TEST_IMAGE_DIR="${CMAKE_PROJECT_SOURCE_DIR}")

