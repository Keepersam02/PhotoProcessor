add_library(utils io/util.cpp io/util.hpp)
add_executable(io_bench_std_o1 io/io_benchmark_std.cpp)
add_executable(io_bench_std_o3 io/io_benchmark_std.cpp)
add_executable(io_bench_sys_o1 io/io_benchmark_sys.cpp)
add_executable(io_bench_sys_o3 io/io_benchmark_sys.cpp)
add_executable(io_bench_uring_o1 io/io_benchmark_uring.cpp)
add_executable(io_bench_uring_o3 io/io_benchmark_uring.cpp)

target_compile_features(io_bench_std_o1 PUBLIC cxx_std_23)
target_compile_features(io_bench_std_o3 PUBLIC cxx_std_23)
target_compile_features(io_bench_sys_o1 PUBLIC cxx_std_23)
target_compile_features(io_bench_sys_o3 PUBLIC cxx_std_23)
target_compile_features(io_bench_uring_o1 PUBLIC cxx_std_23)
target_compile_features(io_bench_uring_o3 PUBLIC cxx_std_23)

target_compile_features(utils PUBLIC cxx_std_23)

target_compile_options(io_bench_std_o1 PRIVATE -g -O1 -fno-omit-frame-pointer -Wall -Wextra -Werror)
target_compile_options(io_bench_std_o3 PRIVATE -g -O3 -fno-omit-frame-pointer -Wall -Wextra -Werror)
target_compile_options(io_bench_sys_o1 PRIVATE -g -O1 -fno-omit-frame-pointer -Wall -Wextra -Werror)
target_compile_options(io_bench_sys_o3 PRIVATE -g -O3 -fno-omit-frame-pointer -Wall -Wextra -Werror)
target_compile_options(io_bench_uring_o1 PRIVATE -g -O1 -fno-omit-frame-pointer -Wall -Wextra -Werror)
target_compile_options(io_bench_uring_o3 PRIVATE -g -O3 -fno-omit-frame-pointer -Wall -Wextra -Werror)
target_compile_options(utils PRIVATE -g -O1 -fno-omit-frame-pointer -Wall -Wextra -Werror)

target_link_libraries(io_bench_std_o1 PRIVATE bpp_lib)
target_link_libraries(io_bench_std_o3 PRIVATE bpp_lib)
target_link_libraries(io_bench_sys_o1 PRIVATE bpp_lib)
target_link_libraries(io_bench_sys_o3 PRIVATE bpp_lib)
target_link_libraries(io_bench_uring_o1 PRIVATE bpp_lib)
target_link_libraries(io_bench_uring_o3 PRIVATE bpp_lib)

target_link_libraries(io_bench_std_o1 PRIVATE utils)
target_link_libraries(io_bench_std_o3 PRIVATE utils)
target_link_libraries(io_bench_sys_o1 PRIVATE utils)
target_link_libraries(io_bench_sys_o3 PRIVATE utils)
target_link_libraries(io_bench_uring_o1 PRIVATE utils)
target_link_libraries(io_bench_uring_o3 PRIVATE utils)


target_link_libraries(utils PRIVATE bpp_lib)

set(MY_TEST_IMAGE_DIR "${PROJECT_SOURCE_DIR}/test/test_images/sample_images")

# Wrap the full definition in quotes, and escape the inner quotes for the C++ string
target_compile_definitions(io_bench_std_o1 PRIVATE "TEST_IMAGE_DIR=\"${MY_TEST_IMAGE_DIR}\"")
target_compile_definitions(io_bench_std_o3 PRIVATE "TEST_IMAGE_DIR=\"${MY_TEST_IMAGE_DIR}\"")
target_compile_definitions(io_bench_sys_o1 PRIVATE "TEST_IMAGE_DIR=\"${MY_TEST_IMAGE_DIR}\"")
target_compile_definitions(io_bench_sys_o3 PRIVATE "TEST_IMAGE_DIR=\"${MY_TEST_IMAGE_DIR}\"")
target_compile_definitions(io_bench_uring_o1 PRIVATE "TEST_IMAGE_DIR=\"${MY_TEST_IMAGE_DIR}\"")
target_compile_definitions(io_bench_uring_o3 PRIVATE "TEST_IMAGE_DIR=\"${MY_TEST_IMAGE_DIR}\"")
target_compile_definitions(utils PRIVATE "TEST_IMAGE_DIR=\"${MY_TEST_IMAGE_DIR}\"")

