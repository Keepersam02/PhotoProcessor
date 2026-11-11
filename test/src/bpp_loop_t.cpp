#include "bpp_loop.hpp"
#include "gtest/gtest.h"

const char test_dir_in_path[750] =
    "/home/keepersam/coding_projects/02-personal/"
    "PhotoProcessor/test/test_dir/01-in";
const char test_dir_out_path[750] =
    "/home/keepersam/coding_projects/02-personal/"
    "PhotoProcessor/test/test_dir/02-out";

TEST(bpp_loop, loop) { bpp_loop(test_dir_in_path, test_dir_out_path); }
