#include "io/in.hpp"
#include "gtest/gtest.h"
#include <cstdio>

const char test_dir_path[750] = "/home/keepersam/coding_projects/02-personal/"
                                "PhotoProcessor/test/test_dir/01-in";

TEST(in, files_info) {
  size_t max_file = 0;
  files_info(test_dir_path, max_file);
}
