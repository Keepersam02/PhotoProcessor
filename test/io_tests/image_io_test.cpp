#include "../../src/io/image_io.hpp"
#include "../../src/io/image_io_error.hpp"
#include "gtest/gtest.h"
#include <array>
#include <cstddef>
#include <expected>
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
  bool ret_err;
  std::array<std::byte, 16> file_header;
};
} // namespace

std::vector<image_io_test_case> test_cases{
    {"tiff_non_raw",
     TIFF_FILE,
     false,
     false,
     {std::byte(0x49), std::byte(0x49), std::byte(0x2A), std::byte(0x00)}},
    {"tiff_raw",
     TIFF_FILE,
     true,
     false,
     {std::byte(0x49), std::byte(0x49), std::byte(0x2A), std::byte(0x00)}}};

TEST(image_io_verification, image_format) {
  fs::path temp_dir = fs::temp_directory_path();
  for (image_io_test_case t_case : test_cases) {
    std::ofstream file(temp_dir / t_case.file_name, std::ios::binary);
    file.write(reinterpret_cast<const char *>(t_case.file_header.data()),
               t_case.file_header.size());
    file.close();
  }

  for (image_io_test_case t_case : test_cases) {
    auto i_type = image_format(fs::path(temp_dir / t_case.file_name).string());
    ASSERT_EQ(i_type, t_case.expected_type);
  }
}

TEST(image_io_verification, is_raw_tiff) {

  std::vector<image_io_test_case> test_cases{
      {"raw_image", RAW_FILE, true, false, {}},
      {"non_raw_image", TIFF_FILE, false, false, {}}};

  fs::path test_dir = fs::temp_directory_path();
  for (image_io_test_case t_case : test_cases) {
    fs::path file_path(test_dir / t_case.file_name);
    TIFF *new_im = TIFFOpen(file_path.c_str(), "w");
    if (!new_im) {
      std::cerr << "Failed to write test file: " << t_case.file_name
                << std::endl;
      continue;
    }
    TIFFSetField(new_im, TIFFTAG_IMAGEWIDTH, 50);
    TIFFSetField(new_im, TIFFTAG_IMAGELENGTH, 50);

    TIFFSetField(new_im, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(new_im, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(new_im, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

    TIFFSetField(new_im, TIFFTAG_ROWSPERSTRIP, 50);

    TIFFSetField(new_im, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

    TIFFCheckpointDirectory(new_im);

    if (t_case.is_raw) {
      //    {TIFFTAG_EP_CFAREPEATPATTERNDIM, 2, 2, TIFF_SHORT, 0,
      //    TIFF_SETGET_C0_UINT16,  FIELD_CUSTOM, 1, 0, "EP
      //    CFARepeatPatternDim", NULL} {TIFFTAG_EP_CFAPATTERN, -1, -1,
      //    TIFF_BYTE, 0, TIFF_SETGET_C16_UINT8,  FIELD_CUSTOM, 1, 1, "EP
      //    CFAPattern", NULL},
      uint8_t cfa_pattern[] = {0, 1, 1, 2};
      uint16_t cfa_repeat_dim[] = {2, 2};
      uint32_t active_area[] = {4, 4, 3004, 4004};
      TIFFSetField(new_im, TIFFTAG_CFAPATTERN, 4, cfa_pattern);
      TIFFSetField(new_im, TIFFTAG_CFAREPEATPATTERNDIM, cfa_repeat_dim);
      TIFFSetField(new_im, TIFFTAG_ACTIVEAREA, active_area);
    }
    TIFFClose(new_im);
  }
  for (image_io_test_case t_case : test_cases) {
    fs::path file_path = test_dir / t_case.file_name;
    auto res = is_raw_tiff(file_path.c_str());
    EXPECT_EQ(!res.has_value(), t_case.ret_err);
    if (t_case.ret_err) {

    } else {
      ASSERT_EQ(res, t_case.is_raw);
    }
  }
}

TEST(image_io_verification, is_raw) {
  std::vector<image_io_test_case> test_cases{
      {"/home/keepersam02/coding_projects/02-personal/PhotoProcessor/test/"
       "test_images/sample_images/_DSF7242.RAF",
       RAW_FILE,
       true,
       false,
       {std::byte('0')}},
      {"/home/keepersam02/coding_projects/02-personal/PhotoProcessor/test/"
       "test_images/sample_images/_DSF7226.RAF",
       RAW_FILE,
       true,
       false,
       {std::byte('0')}}};

  for (image_io_test_case t_case : test_cases) {
    auto res = is_raw_file(t_case.file_name);
    EXPECT_EQ(t_case.ret_err, !res.has_value());
    if (t_case.ret_err) {

    } else {
      ASSERT_EQ(res, t_case.is_raw);
    }
  }
}
