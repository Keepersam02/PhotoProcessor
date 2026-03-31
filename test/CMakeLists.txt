add_executable(bpp_tests
  ./io_tests/image_ver_test.cpp
)

target_compile_options(bpp_tests PRIVATE -g -O0)
set(CMAKE_BUILD_TYPE Debug)

set(ASAN_FLAGS -fsanitize=address -fno-omit-frame-pointer -g)
# 2. Check if we are in Debug mode
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Build Type is Debug: Enabling AddressSanitizer (ASan)")

  # Apply to your MAIN LIBRARY (bpp_lib)
  target_compile_options(bpp_tests PUBLIC ${ASAN_FLAGS})
  target_link_options(bpp_tests PUBLIC -fsanitize=address)
endif()

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


target_compile_definitions(bpp_tests PRIVATE TEST_IMAGE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test_images")
