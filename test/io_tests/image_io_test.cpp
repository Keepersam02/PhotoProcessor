#include "../../src/io/image_io.hpp"
#include "../../src/io/image_io_error.hpp"
#include "gtest/gtest.h"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <tiff.h>
#include <tiffio.h>
#include <vector>

namespace fs = std::filesystem;

namespace {
struct image_io_test_case {
  std::string file_name;
  image_type expected_type;
  bool is_raw;
  std::array<std::byte, 16> file_header;
};
} // namespace

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

TEST(image_io, is_raw_tiff) {

  std::vector<image_io_test_case> test_cases{
      {"raw_image", RAW_FILE, true, {}},
      {"non_raw_image", TIFF_FILE, false, {}}};

  fs::path test_dir = fs::temp_directory_path();
  for (image_io_test_case t_case : test_cases) {
    fs::path file_path(test_dir / t_case.file_name);
    TIFF *new_im = TIFFOpen(file_path.c_str(), "w");
    if (!new_im) {
      std::cerr << "Failed to write test file: " << t_case.file_name
                << std::endl;
    }
    TIFFSetField(new_im, TIFFTAG_IMAGEWIDTH, 50);
    TIFFSetField(new_im, TIFFTAG_IMAGELENGTH, 50);
    TIFFSetField(new_im, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(new_im, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(new_im, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

    if (t_case.is_raw) {
      int cfa_pattern[] = {0, 1, 1, 2};
      int cfa_repeat_dim[] = {2, 2};
      int active_area[] = {4, 4, 3004, 4004};
      TIFFSetField(new_im, TIFFTAG_CFAPATTERN, cfa_pattern);
      TIFFSetField(new_im, TIFFTAG_CFAREPEATPATTERNDIM, cfa_repeat_dim);
      TIFFSetField(new_im, TIFFTAG_ACTIVEAREA, active_area);
    }
    TIFFClose(new_im);
  }
  for (image_io_test_case t_case : test_cases) {
    fs::path file_path = test_dir / t_case.file_name;
    bool res = is_raw_tiff(file_path.c_str());
    ASSERT_EQ(res, t_case.is_raw);
  }
}

TEST(raw_image_io, is_raw) {}
