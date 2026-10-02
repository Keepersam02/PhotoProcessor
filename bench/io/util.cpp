#include "util.hpp"
#include "types/image_files.hpp"
#include <cstdint>
#include <cstdio>
#include <fcntl.h>
#include <iostream>
#include <memory>
#include <sys/stat.h>
#include <unistd.h>

std::unique_ptr<image_files> create_image_files(fs::path test_dir,
                                                unsigned &largest_size) {
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
  auto im_files = std::make_unique<image_files>(num_files);
  uint64_t cur_index = 0;
  for (const auto &dir_entry : std::filesystem::directory_iterator(test_dir)) {
    if (dir_entry.is_regular_file()) {
      im_files->files[cur_index] = image_file(dir_entry, cur_index);
      cur_index++;
    } else {
      std::cerr << "entry is not regular file, path: " << dir_entry.path()
                << std::endl;
    }
  }
  return im_files;
}
void file_pool_plucker(file_pool &pool, image_files &images) {
  size_t num_files_poped = 0;
  while (images.get_pre_proc_size() != images.num_files) {

    uint64_t index = images.get_loaded();
    if (index == UINT64_MAX) {
      continue;
    }
    auto im = &images.files[index];
    pool.free_slot(im->file_data);
    images.inspro_proc(index);
    num_files_poped++;
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  std::cout << num_files_poped;
}
