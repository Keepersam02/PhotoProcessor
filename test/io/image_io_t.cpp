#include "image_handling/file_pool.hpp"
#include "io/image_io.hpp"
#include "types/image_files.hpp"
#include "gtest/gtest.h"
#include <cstddef>
#include <filesystem>
#include <iterator>
#include <memory>

namespace fs = std::filesystem;

class io_test : public testing::Test {
public:
  size_t num_files;
  fs::path src_dir;
  std::unique_ptr<image_files> images;
  std::unique_ptr<file_pool> pool;

  void SetUp() override {
    if (!fs::exists(src_dir)) {
      std::cerr << "Failed to init image_io_t test, path does not exist"
                << src_dir.c_str();
      return;
    }
    if (!fs::is_directory(src_dir)) {
      std::cerr << "Failed init image_io_test, provided path not a directory"
                << src_dir.c_str();
      return;
    }

    fs::directory_iterator iter(src_dir);
    const auto num_files_ = std::distance(iter, fs::directory_iterator{});
    images = std::make_unique<image_files>(num_files_);
    size_t i = 0;
    size_t cur_max = 0;
    for (const auto path : iter) {
      images->files[i].file_path_ = path.path();
      if (fs::file_size(path.path()) > cur_max) {
        cur_max = fs::file_size(path.path());
      }
    }
    pool = std::make_unique<file_pool>(cur_max, .4f);
  }

  void TearDown() override {}
};

TEST_F(io_test, image_io_std) {
  bool res = import_images_std(*images, *pool);
  EXPECT_EQ(res, 0);
  EXPECT_EQ(images->loaded.size(), num_files);
}
