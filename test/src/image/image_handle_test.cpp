#include "image_handling/image_handler.hpp"
#include "io/image_io.hpp"
#include "io/image_io_error.hpp"

#include <filesystem>
#include <format>
#include <gtest/gtest.h>
#include <string>
#include <utility>

namespace fs = std::filesystem;

struct handler_test_case {
  std::string t_name;
  std::string file_name;
  std::string dest_dir_name;
  fs::path test_file_path;
  std::string file_hash;
  size_t file_size;
};

class image_handle_test_fixture : public testing::Test {
protected:
  std::vector<handler_test_case> handler_cases = {
      {"fuji raw photo 1", "_DSF7215", "export1", "null",
       "351c3e755730256e59577d8e54251306", 58801472},
      {"fuji raw photo 2", "_DSF7226", "export1", "null",
       "54b39742b464fe9af609d304a17f7ea6", 59019072},
      {"fuji raw photo 3", "_DSF7242", "export1", "null",
       "e61c04bf3275756a403d372e6fecca5e", 58881856}};
  std::vector<fs::path> file_paths;
  std::vector<std::pair<fs::path, size_t>> loaded_files;

  void SetUp() override {
    fs::path test_dir = fs::temp_directory_path() / "bpp_test2";
    fs::create_directory(test_dir);

    for (const auto t_case : handler_cases) {
      file_paths.push_back(TEST_IMAGE_DIR / fs::path(t_case.file_name));
    }
    std::vector<std::pair<fs::path, image_error>> copy_failures;
    auto val = file_loader(file_paths, copy_failures);
    if (!val) {
      // crash
      std::cout << "failed to load files into memory";
      ASSERT_EQ(true, false);
    }
    if (val->size() != handler_cases.size()) {
      std::cout << std::format(
          "improper number of files loaded, expected: {}, got: {}", val->size(),
          handler_cases.size());
      ASSERT_EQ(true, false);
    }
  }
};

TEST_F(image_handle_test_fixture, file_copier) {
  std::cout << file_paths.at(0).c_str();
}
