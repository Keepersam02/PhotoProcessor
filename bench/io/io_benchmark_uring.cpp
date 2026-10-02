
#include "io/image_io.hpp"
#include "util.hpp"
#include <atomic>
#include <fcntl.h>
#include <filesystem>
#include <functional>
#include <future>
#include <iostream>
#include <ostream>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>

namespace fs = std::filesystem;

int main() {
  fs::path test_im_dir = fs::path(TEST_IMAGE_DIR);

  unsigned largest_file;
  auto ims = create_image_files(test_im_dir, largest_file);
  fs::path err_path = fs::path(TEST_IMAGE_DIR) / "bench/error.txt";
  float percent_ram = .1;
  file_pool pool(largest_file, percent_ram);
  std::future<bool> import_res = std::async(
      std::launch::async, import_images_uring, std::ref(*ims), std::ref(pool));

  std::thread popper(file_pool_plucker, std::ref(pool), std::ref(*ims));

  bool im_res = import_res.get();
  popper.join();

  std::cout << im_res;
  return 0;
}
