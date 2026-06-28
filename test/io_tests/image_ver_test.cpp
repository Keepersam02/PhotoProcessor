#include "../../src/io/image_io.cpp"
#include "../../src/io/image_ver.hpp"
#include "io/image_io_error.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <array>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <ostream>
#include <string>
#include <system_error>
#include <tiff.h>
#include <tiffio.h>
#include <vector>

namespace fs = std::filesystem;

struct im_ver_tcase {
  std::string name;
  bool is_image;
  bool is_raw;
  bool fake_tiff;
  fs::path sub_dir;
  fs::path image_file_path;
  std::string non_image_write;
  image_type image_type_;
  bool ret_err;
  image_error image_error_;
};

std::vector<im_ver_tcase> test_cases{
    {"non_raw_nonest1.txt", false, false, false, fs::path(), fs::path(),
     "blah blah blah", image_type::INTER_IM, false,
     image_error::IO(0, err_severity::DEBUG, "", "")},
    {"non_raw_nonest2.txt", false, false, false, fs::path("sub_dir1"),
     fs::path(), "also blah", image_type::INTER_IM, false,
     image_error::IO(0, err_severity::DEBUG, "", "")},
    {"fake_tiff_raw1.tiff", true, true, true, fs::path(), fs::path(), "",
     image_type::TIFF_FILE, false,
     image_error::IO(0, err_severity::DEBUG, "", "")},
    {"fake_tiff_nonraw1.tiff", true, false, true, fs::path(), fs::path(), "",
     image_type::TIFF_FILE, false,
     image_error::IO(0, err_severity::DEBUG, "", "")},
    {"_DSF7215.RAF", true, true, false, fs::path(),
     fs::path(TEST_IMAGE_DIR) / "_DSF7215.RAF", "", image_type::INTER_IM, false,
     image_error::IO(0, err_severity::DEBUG, "", "")},
    {"_DSF7226.RAF", true, true, false, fs::path(),
     fs::path(TEST_IMAGE_DIR) / "_DSF7226.RAF", "", image_type::INTER_IM, false,
     image_error::IO(0, err_severity::DEBUG, "", "")}};

class image_ver_tf : public testing::Test {
protected:
  fs::path root_test_dir;
  std::vector<im_ver_tcase> all_tfile;
  std::vector<im_ver_tcase> raw_files;
  std::vector<im_ver_tcase> image_files;

private:
  void SetUp() override {
    root_test_dir = fs::path(fs::temp_directory_path()) / "bpp_root_test_dir";
    fs::create_directory(root_test_dir);
    for (const auto &t_case : test_cases) {
      fs::path sub_dir = fs::path(root_test_dir) / t_case.sub_dir;
      if (!fs::exists(sub_dir)) {
        fs::create_directories(sub_dir);
      }
      all_tfile.push_back(t_case);
      if (!t_case.is_image) {
        const auto ret = non_image_writer(t_case);
        if (!ret) {
          // call ceanup?
          return;
        }
      } else if (t_case.is_image && t_case.is_raw && t_case.fake_tiff) {
        const bool res = fake_raw_tiff_writer(t_case);
        if (!res) {
          // cleanup
          return;
        }
        image_files.push_back(t_case);
      } else if (t_case.is_image && !t_case.is_raw && t_case.fake_tiff) {
        const bool res = fake_nraw_tiff_writer(t_case);
        if (!res) {
          // cleanup
          return;
        }
        image_files.push_back(t_case);
      } else if (t_case.is_image && t_case.is_raw && !t_case.fake_tiff) {
        const bool res = raw_writer(t_case);
        if (!res) {
          // cleanup
          return;
        }
        raw_files.push_back(t_case);
        image_files.push_back(t_case);
      } else {
        // cleanup something went wrong
      }
    }
  }

  void TearDown() override { fs::remove_all(root_test_dir); }

  bool non_image_writer(const im_ver_tcase &t_case) {
    const auto file_path =
        fs::path(root_test_dir) / t_case.sub_dir / t_case.name;
    std::ofstream file_out(file_path, std::ios::out);
    if (!file_out.is_open()) {
      const auto err = std::system_category().message(errno);
      std::cout << "failed to open file: " << file_path.c_str()
                << "message: " << err;
      return false;
    }
    file_out.write(t_case.non_image_write.c_str(),
                   t_case.non_image_write.size());
    if (file_out.tellp() != t_case.non_image_write.size()) {
      std::cout << std::system_category().message(errno);
      file_out.close();
      return false;
    }
    file_out.close();
    return true;
  }

  bool fake_raw_tiff_writer(const im_ver_tcase &t_case) {
    const auto f_path = fs::path(root_test_dir) / t_case.sub_dir / t_case.name;
    TIFF *file = TIFFOpen(f_path.c_str(), "w");
    if (file == NULL) {
      std::cout << std::system_category().message(errno);
      return false;
    }
    TIFFSetField(file, TIFFTAG_IMAGEWIDTH, 50);
    TIFFSetField(file, TIFFTAG_IMAGELENGTH, 50);
    TIFFSetField(file, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(file, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(file, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(file, TIFFTAG_ROWSPERSTRIP, 50);
    TIFFSetField(file, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
    TIFFCheckpointDirectory(file);

    uint8_t cfa_pattern[] = {0, 1, 1, 2};
    uint16_t cfa_repeat_dim[] = {2, 2};
    uint32_t active_area[] = {4, 4, 3004, 4004};
    TIFFSetField(file, TIFFTAG_CFAPATTERN, 4, cfa_pattern);
    TIFFSetField(file, TIFFTAG_CFAREPEATPATTERNDIM, cfa_repeat_dim);
    TIFFSetField(file, TIFFTAG_ACTIVEAREA, active_area);

    std::vector<uint8_t> dummy(50, 0);
    TIFFWriteScanline(file, dummy.data(), 0, 0);
    TIFFClose(file);

    return true;
  }

  bool fake_nraw_tiff_writer(const im_ver_tcase &t_case) {
    const auto f_path = fs::path(root_test_dir) / t_case.sub_dir / t_case.name;
    TIFF *file = TIFFOpen(f_path.c_str(), "w");
    if (file == NULL) {
      std::cout << std::system_category().message(errno);
      return false;
    }
    TIFFSetField(file, TIFFTAG_IMAGEWIDTH, 50);
    TIFFSetField(file, TIFFTAG_IMAGELENGTH, 50);
    TIFFSetField(file, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(file, TIFFTAG_SAMPLESPERPIXEL, 1);
    TIFFSetField(file, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(file, TIFFTAG_ROWSPERSTRIP, 50);
    TIFFSetField(file, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

    std::vector<uint8_t> dummy(50, 0);
    TIFFWriteScanline(file, dummy.data(), 0, 0);
    TIFFClose(file);
    return true;
  }

  bool raw_writer(const im_ver_tcase &t_case) {
    const fs::path out_path =
        fs::path(root_test_dir) / t_case.sub_dir / t_case.name;
    std::ifstream file_in(t_case.image_file_path, std::ios::binary);
    if (!file_in.is_open()) {
      const auto err = std::system_category().message(errno);
      std::cout << err;
      return false;
    }
    file_in.seekg(0, std::ios::end);
    size_t file_size = file_in.tellg();
    if (file_size == -1) {
      std::cout << std::system_category().message(errno);
      file_in.close();
      return false;
    }
    file_in.seekg(std::ios::beg);
    char *buffer = new char[file_size];
    file_in.read(buffer, file_size);
    if (file_in.tellg() != file_size) {
      std::cout << std::system_category().message(errno);
      file_in.close();
      delete[] buffer;
      return false;
    }
    file_in.close();

    std::ofstream file_out(out_path, std::ios::binary);
    if (!file_out.is_open()) {
      const auto err = std::system_category().message(errno);
      std::cout << err;
      return false;
    }
    file_out.write(buffer, file_size);
    if (file_out.tellp() != file_size) {
      std::cout << std::system_category().message(errno);
      file_out.close();
      delete[] buffer;
      return false;
    }
    file_out.close();
    delete[] buffer;
    return true;
  }
};

TEST_F(image_ver_tf, sort_file) {
  std::vector<fs::path> image_paths;
  image_paths.reserve(image_files.size());
  for (const auto &t_case : image_files) {
    image_paths.push_back(fs::path(root_test_dir) / t_case.sub_dir /
                          t_case.name);
  }
  std::vector<fs::path> all_fpaths;
  all_fpaths.reserve(all_tfile.size());
  for (const auto t_case : all_tfile) {
    all_fpaths.push_back(fs::path(root_test_dir) / t_case.sub_dir /
                         t_case.name);
  }
  const auto ret = sort_file(all_fpaths);
  if (!ret) {
    // doesnt happen for now
  }
  EXPECT_EQ(all_fpaths.size(), image_files.size());
  for (const auto &path : all_fpaths) {
    EXPECT_THAT(image_paths, ::testing::Contains(path));
  }
}

TEST_F(image_ver_tf, image_format) {
  for (const auto &t_case : image_files) {
    auto res =
        image_format(fs::path(root_test_dir) / t_case.sub_dir / t_case.name);
    EXPECT_EQ(t_case.ret_err, !res.has_value());
    if (t_case.ret_err) {

      continue;
    }
    EXPECT_EQ(res.value(), t_case.image_type_);
  }
}

TEST_F(image_ver_tf, is_raw_tiff) {
  for (const auto &t_case : image_files) {
    if (!t_case.fake_tiff) {
      continue;
    }
    const auto res =
        is_raw_tiff(fs::path(root_test_dir) / t_case.sub_dir / t_case.name);
    EXPECT_EQ(!res.has_value(), t_case.ret_err);
    if (t_case.ret_err) {
    }
    const auto &raw_pair = res.value();
    EXPECT_EQ(raw_pair.first, t_case.is_raw) << raw_pair.second;
  }
}

TEST_F(image_ver_tf, is_raw_file) {
  for (const auto &t_case : raw_files) {
    const auto &res =
        is_raw_file(fs::path(root_test_dir) / t_case.sub_dir / t_case.name);
    EXPECT_EQ(!res.has_value(), t_case.ret_err) << t_case.name;
    if (t_case.ret_err) {
    }
    const auto &raw_pair = res.value();
    EXPECT_EQ(raw_pair.first, t_case.is_raw) << t_case.name;
  }
}
