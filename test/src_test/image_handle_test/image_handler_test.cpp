#include "image_handling/image_handler.hpp"
#include "io/image_io.hpp"
#include "io/image_io_error.hpp"

#include <cstddef>
#include <filesystem>
#include <format>
#include <gtest/gtest.h>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

struct im_handle_tcase {
  std::string case_name;
  std::string file_name;
  fs::path test_file_path;
  fs::path dest_dir;
  void *file_mem;
  size_t file_size;
  std::string file_hash;
};

class im_handler_tf : public testing::Test {
protected:
  std::vector<im_handle_tcase> t_cases;

  void SetUp() override {
    std::vector<fs::path> test_paths{
        {fs::path(TEST_IMAGE_DIR) / "_DSF7215"},
        {fs::path(TEST_IMAGE_DIR) / "_DSF7226"},
        {fs::path(TEST_IMAGE_DIR) / "_DSF7242"},
        {fs::path(TEST_IMAGE_DIR) / "_DSF7249"},
    };

    std::vector<std::pair<void *, size_t>> loaded_files;
    {
      std::vector<std::pair<fs::path, image_error>> failed_loads;
      auto loaded_files_t = file_loader(test_paths, failed_loads);
      if (!loaded_files_t || loaded_files_t->size() != test_paths.size()) {
        std::string fail_mess;
        fail_mess.reserve(1024);
        for (const auto &fail : failed_loads) {
          fail_mess +=
              std::format("File {}, Failed: {}, {}\n", fail.first.c_str(),
                          fail.second.message_, fail.second.extra_info_);
        }
        std::cout << fail_mess;
        return;
      } else {
        loaded_files = std::move(loaded_files_t.value());
      }
    }
    fs::path export_dir = fs::temp_directory_path() / "im_handler_test";
    for (int i = 0; i < test_paths.size(); i++) {
      const auto path = &test_paths.at(i);
      const auto file_pair = &loaded_files.at(i);
      t_cases.push_back(im_handle_tcase{path->filename(), path->filename(),
                                        *path, export_dir, file_pair->first,
                                        file_pair->second, "temp_null"});
    }
  }
};

TEST_F(im_handler_tf, file_copier_test) {
  for (const auto &t_case : t_cases) {
    auto test_ret = file_copier(t_case.file_mem, t_case.file_size,
                                t_case.test_file_path, t_case.dest_dir);
    ASSERT_EQ(test_ret.has_value(), true);
  }
}
