#include "../../src/io/image_io.cpp"
#include "../../src/io/image_ver.hpp"
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
};

std::vector<im_ver_tcase> test_cases{
    {"non_raw_nonest1.txt", false, false, false, fs::path(), fs::path(),
     "blah blah blah"},
    {"non_raw_nonest2.txt", false, false, false, fs::path("sub_dir1"),
     fs::path(), "also blah"},
    {"fake_tiff_raw1.tiff", true, true, true, fs::path(), fs::path(), ""},
    {"fake_tiff_nonraw1.tiff", true, false, true, fs::path(), fs::path(), ""},
    {"_DSF7215.RAF", true, true, false, fs::path(),
     fs::path(TEST_IMAGE_DIR) / "_DSF7215.RAF", ""},
    {"_DSF7226.RAF", true, true, false, fs::path(),
     fs::path(TEST_IMAGE_DIR) / "_DSF7226.RAF", ""}};

class image_ver_tf : public testing::Test {
protected:
  fs::path root_test_dir;
  std::vector<fs::path> all_tfile;
  std::vector<fs::path> raw_files;
  std::vector<fs::path> image_files;

private:
  void SetUp() override {
    root_test_dir = fs::path(fs::temp_directory_path()) / "bpp_root_test_dir";
    for (const auto &t_case : test_cases) {
      if (!t_case.is_image) {
        const auto ret = non_image_writer(t_case);
        if (!ret) {
          // call ceanup?
          return;
        }
      } else if (t_case.is_image && !t_case.is_raw && t_case.fake_tiff) {
      }
    }
  }

  bool non_image_writer(const im_ver_tcase &t_case) {
    const auto file_path =
        fs::path(root_test_dir) / t_case.sub_dir / t_case.name;
    std::ofstream file_out(file_path, std::ios::out);
    file_out.write(t_case.non_image_write.c_str(),
                   t_case.non_image_write.size());
    if (file_out.tellp() != t_case.non_image_write.size()) {
      return false;
    }
    file_out.close();
    return true;
  }

  bool non_image_ftiff_writer(im_ver_tcase &t_case) {
    const auto f_path = fs::path(root_test_dir) / t_case.sub_dir / t_case.name;
    Tiff *file = TIFFOpen(f_path.c_str(), 'w');
    if (file == NULL) {
      std::cout << std::system_category().message(errno);
      return false;
    }
  }
};
