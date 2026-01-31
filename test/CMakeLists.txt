add_executable(bpp_tests
  ./io_tests/image_io_test.cpp
)

target_compile_options(bpp_tests PRIVATE -g -O0)
set(CMAKE_BUILD_TYPE Debug)

include(FetchContent)
FetchContent_Declare(
  GTest
  URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)
FetchContent_MakeAvailable(GTest)

target_link_libraries(bpp_tests PRIVATE
  bpp_lib
  gtest
  gtest_main
  gmock
  gmock_main
)
