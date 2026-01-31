#include "../../src/io/image_io.hpp"
#include "gtest/gtest.h"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <vector>

namespace fs = std::filesystem;

typedef struct {
  std::string file_name;
  image_type expected_type;
  image_type returned_type;
  bool is_raw;
  std::array<std::byte, 16> file_header;
} image_io_test_case;

TEST(image_io, image_format) {}

TEST(image_io, is_raw_tiff) {}
