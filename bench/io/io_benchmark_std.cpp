#include "io/image_io.hpp"
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <system_error>

namespace fs = std::filesystem;

image_files create_image_files(fs::path test_dir) {
  fs::directory_entry dir(test_dir);
  if (!dir.is_directory()) {
    std::cerr << "non directory passed to find test files, path: " << dir.path()
              << std::endl;
  }
  uint64_t num_files = 0;
  for (const auto &entry : std::filesystem::directory_iterator(test_dir)) {
    (void)entry;
    num_files++;
  }
  image_files im_files(num_files);
  uint64_t cur_index = 0;
  for (const auto &dir_entry : std::filesystem::directory_iterator(test_dir)) {
    if (dir_entry.is_regular_file()) {
      im_files.files_[cur_index] = image_file(dir_entry, cur_index);
      cur_index++;
    } else {
      std::cerr << "entry is not regular file, path: " << dir_entry.path()
                << std::endl;
    }
  }
  return im_files;
}

int main() {
  fs::path test_im_dir(
      "/home/keepersam02/coding_projects/02-personal/PhotoProcessor/test/"
      "test_images/sample_images/");

  auto ims = create_image_files(test_im_dir);
  fs::path err_path("/home/keepersam02/coding_projects/02-personal/"
                    "PhotoProcessor/bench/error.txt");
  auto res = import_images_std(ims, err_path);
  if (res != true) {
    return -1;
  }
  return 0;
}
