#include "../../src/io/image_io.cpp"
#include "../../src/io/image_io_error.hpp"
#include "../../src/io/image_ver.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <fstream>
#include <ios>
#include <ostream>
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
      {std::format("{}/_DSF7242.RAF", TEST_IMAGE_DIR),
       RAW_FILE,
       true,
       false,
       {std::byte('0')}},
      {std::format("{}/_DSF7226.RAF", TEST_IMAGE_DIR),
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

namespace {
struct test_file {
  std::string name;
  fs::path file_path;
  bool is_dir;
  bool is_reg;
  bool is_valid;
  bool is_nested;
  // todo add more for stuff like a file hash...
};

struct io_test_case {
  std::string name;
  int num_succ_nest;
  int num_rej_nest;
  int num_succ_no_nest;
  int num_rej_no_nest;
  int num_file_no_nest;
  std::vector<test_file> files;
};
} // namespace

class ImageIOFixture : public testing::Test {
protected:
  fs::path master_dir;
  std::vector<io_test_case> test_cases;

  void SetUp() override {
    master_dir = fs::temp_directory_path() / "bpp_test";
    fs::create_directory(master_dir);
    std::vector<io_test_case> t_cases{
        {"no_nest_all_valid",
         2,
         0,
         2,
         0,
         2,
         {{"_DSF7242.RAF",
           fs::path(master_dir) / "no_nest_all_valid" / "_DSF7242.RAF", false,
           true, true, false},
          {"_DSF7226.RAF",
           fs::path(master_dir) / "no_nest_all_valid" / "_DSF7226.RAF", false,
           true, true, false}}},
        {"no_nest_no_valid",
         0,
         2,
         0,
         2,
         2,
         {{"rand.txt", fs::path(master_dir) / "no_nest_no_valid" / "rand.txt",
           false, true, false, false},
          {"rand2.txt", fs::path(master_dir) / "no_nest_no_valid" / "rand2.txt",
           false, true, false, false}}},
        {"no_nest_mix",
         2,
         1,
         2,
         1,
         3,
         {{"rand.txt", fs::path(master_dir) / "no_nest_mix" / "rand.txt", false,
           true, false, false},
          {"_DSF7226.RAF",
           fs::path(master_dir) / "no_nest_mix" / "_DSF7226.RAF", false, true,
           true, false},
          {"_DSF7242.RAF",
           fs::path(master_dir) / "no_nest_mix" / "_DSF7242.RAF", false, true,
           true, false}}}};

    for (const io_test_case t_case : t_cases) {
      fs::path test_path = fs::path(master_dir) / t_case.name;
      if (!fs::create_directory(test_path)) {
        std::cout << "failed to create test directory: " << t_case.name
                  << std::endl;
        continue;
      }
      for (const test_file file : t_case.files) {
        fs::path file_path = fs::path(test_path) / file.name;
        if (file.is_dir) {
          fs::create_directory(file_path);
          continue;
        } else if (!file.is_valid && file.is_reg) {
          std::ofstream ostream(file_path, std::ios::out);
          std::string message = "in a bottle, some bs to make this long enough "
                                "to read properly so it doesnt return weirdly";
          ostream.write(message.data(), message.size());
        } else if (file.is_valid) {
          CopyImage(file.name, file_path);
        }
      }
    }
    test_cases = t_cases;
  }

  void TearDown() override { fs::remove_all(master_dir); }

private:
  bool CopyImage(std::string name, fs::path dest) {
    fs::path image_src = fs::path(TEST_IMAGE_DIR) / name;
    std::ifstream istream(image_src, std::ios::binary);
    istream.seekg(0, std::ios::end);
    size_t file_size = istream.tellg();
    EXPECT_NE(file_size, -1) << "read directory as file: " << dest.string();
    istream.seekg(std::ios::beg);

    char *buffer = new char[file_size];
    istream.read(buffer, file_size);
    if (file_size != istream.tellg()) {
      std::cout << "failed to read test image: " << image_src.string()
                << std::endl;
      delete[] buffer;
      return false;
    }

    std::ofstream ostream(dest, std::ios::binary);
    ostream.write(buffer, file_size);
    if (ostream.tellp() != file_size) {
      std::cout << "failed to write test file: " << dest.string() << std::endl;
      delete[] buffer;
      return false;
    }
    delete[] buffer;
    return true;
  }
};

TEST_F(ImageIOFixture, image_io_find_files) {
  for (const io_test_case &t_case : test_cases) {
    fs::path test_path = fs::path(master_dir) / t_case.name;
    auto ret = find_files(test_path);
    ASSERT_EQ(ret.has_value(), true)
        << "expected return value doesnt exist, test name: "
        << t_case.name; // todo change after updating test struct

    auto value = ret.value();
    ASSERT_EQ(value.size(), t_case.num_file_no_nest)
        << "expected size does not match ret, test: " << t_case.name
        << " wanted: " << t_case.files.size();

    for (const auto &file : t_case.files) {
      if (file.is_dir || file.is_reg && !file.is_nested) {
        ASSERT_THAT(value, ::testing::Contains(file.file_path));
      }
    }
  }
}

TEST_F(ImageIOFixture, image_io_sort_file) {
  for (auto const &t_case : test_cases) {
    std::vector<fs::path> file_paths;
    for (auto const &file : t_case.files) {
      file_paths.push_back(file.file_path);
    }
    auto res = sort_file(file_paths);
    ASSERT_EQ(res.has_value(), true);
    auto val = res.value();
    ASSERT_EQ(val.size(), t_case.num_rej_nest) << t_case.name;
    ASSERT_EQ(file_paths.size(), t_case.num_succ_nest) << t_case.name;
  }
}

TEST_F(ImageIOFixture, file_loader) {
  for (auto const &t_case : test_cases) {
    std::vector<fs::path> file_paths;
    for (auto const &file : t_case.files) {
      if (file.is_valid) {
        file_paths.push_back(file.file_path);
      }
    }
    std::vector<std::pair<fs::path, image_error>> failed;
    auto res = file_loader(file_paths, failed);
    EXPECT_EQ(res.has_value(), true);

    auto const res_vec = res.value();
    EXPECT_EQ(res_vec.size(), t_case.num_succ_no_nest) << t_case.name;
  }
}
