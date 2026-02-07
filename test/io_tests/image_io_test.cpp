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
  bool is_raw;
  std::array<std::byte, 16> file_header;
} image_io_test_case;

std::vector<image_io_test_case> test_cases{
    {"tiff_non_raw",
     TIFF_FILE,
     false,
     {std::byte(0x49), std::byte(0x49), std::byte(0x2A), std::byte(0x00)}},
    {"tiff_raw",
     TIFF_FILE,
     true,
     {std::byte(0x49), std::byte(0x49), std::byte(0x2A), std::byte(0x00)}}};

TEST(image_io, image_format) {
  fs::path temp_dir = fs::temp_directory_path();
  for (image_io_test_case t_case : test_cases) {
    std::ofstream file(temp_dir / t_case.file_name, std::ios::binary);
    file.write(reinterpret_cast<const char *>(t_case.file_header.data()),
               t_case.file_header.size());
    file.close();
  }

  for (image_io_test_case t_case : test_cases) {
    image_type i_type =
        image_format(fs::path(temp_dir / t_case.file_name).string());
    ASSERT_EQ(i_type, t_case.expected_type);
  }
}

TEST(image_io, is_raw_tiff) {}
