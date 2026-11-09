add_executable(bpp_tests
  src/io/in_t.cpp
  src/bpp_loop_t.cpp
)

target_compile_options(bpp_tests PRIVATE
  $<$<CONFIG:Debug>:
  -Wall;
  -Wextra;
  -Wpedantic;
  -g;
  -O1
  -Wshadow;
  -Wunused;
  >
)
find_package(GTest)

target_link_libraries(bpp_tests bpp_lib gtest gtest_main)
