add_executable(bpp_tests
  ./io_tests/image_io_test.cpp
)

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
