#include "io/image_io.hpp"
#include <cstdint>
#include <fcntl.h>
#include <filesystem>
#include <iostream>
#include <ostream>
#include <sys/stat.h>
#include <unistd.h>

namespace fs = std::filesystem;

image_files create_image_files(fs::path test_dir, unsigned &largest_size) {
  fs::directory_entry dir(test_dir);
  if (!dir.is_directory()) {
    std::cerr << "non directory passed to find test files, path: " << dir.path()
              << std::endl;
  }
  uint64_t num_files = 0;
  largest_size = 0;
  for (const auto &entry : std::filesystem::directory_iterator(test_dir)) {
    int fd = open(entry.path().c_str(), O_RDONLY);
    struct stat statbuf;
    int ret = fstat(fd, &statbuf);
    if (ret == -1) {
    }
    if (largest_size < statbuf.st_size) {
      largest_size = statbuf.st_size;
    }
    close(fd);
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
  fs::path test_im_dir = fs::path(TEST_IMAGE_DIR) /
                         "test/"
                         "test_images/sample_images/";

  unsigned largest_file;
  auto ims = create_image_files(test_im_dir, largest_file);
  fs::path err_path = fs::path(TEST_IMAGE_DIR) / "bench/error.txt";
  size_t pool_size = 16;
  file_pool pool(largest_file, pool_size);
  auto res = import_images_std(ims, pool, err_path);
  if (res != true) {
    return -1;
  }
  return 0;
}
