add_executable(bpp_tests
  src/io/in_t.cpp
  src/bpp_loop_t.cpp
  src/db/db_manag_t.cpp
)

find_package(GTest)

target_link_libraries(bpp_tests bpp_lib gtest gtest_main)
