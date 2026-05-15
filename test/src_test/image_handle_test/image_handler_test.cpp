#include "image_handling/image_handler.hpp"
#include "io/image_io.hpp"
#include "io/image_io_error.hpp"

#include <cstddef>
#include <filesystem>
#include <format>
#include <fstream>
#include <gtest/gtest.h>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/types.h>
#include <sstream>
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
  fs::path export_dir;
  void SetUp() override {
    std::vector<std::pair<fs::path, std::string>> path_hash{
        {fs::path(TEST_IMAGE_DIR) / "_DSF7215.RAF",
         "351c3e755730256e59577d8e54251306"},
        {fs::path(TEST_IMAGE_DIR) / "_DSF7226.RAF",
         "54b39742b464fe9af609d304a17f7ea6"},
        {fs::path(TEST_IMAGE_DIR) / "_DSF7242.RAF",
         "e61c04bf3275756a403d372e6fecca5e"},
        {fs::path(TEST_IMAGE_DIR) / "_DSF7249.RAF",
         "359d45d1bbcb94fd66a2000e94984670"},
    };
    std::vector<fs::path> test_paths;
    for (const auto path : path_hash) {
      test_paths.push_back(path.first);
    }

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
    export_dir = fs::temp_directory_path() / "im_handler_test";
    fs::create_directory(export_dir);
    for (int i = 0; i < test_paths.size(); i++) {
      const auto path = path_hash.at(i);
      const auto file_pair = &loaded_files.at(i);
      t_cases.push_back(im_handle_tcase{
          path.first.filename(), path.first.filename(), path.first, export_dir,
          file_pair->first, file_pair->second, path.second});
    }
  }

  void TearDown() override { fs::remove_all(export_dir); }
};

TEST_F(im_handler_tf, file_copier_test) {
  for (const auto &t_case : t_cases) {
    auto test_ret = file_copier(t_case.file_mem, t_case.file_size,
                                t_case.test_file_path, t_case.dest_dir);
    ASSERT_EQ(test_ret.has_value(), true);

    std::ifstream ifstream_(fs::path(export_dir) / t_case.file_name);
    if (!ifstream_.is_open()) {
      // somethign
    }
    char buffer[8192];
    EVP_MD_CTX *context = EVP_MD_CTX_new();
    EVP_DigestInit_ex(context, EVP_md5(), nullptr);

    while (ifstream_.read(buffer, sizeof(buffer))) {
      EVP_DigestUpdate(context, buffer, ifstream_.gcount());
    }

    if (ifstream_.gcount() > 0) {
      EVP_DigestUpdate(context, buffer, ifstream_.gcount());
    }

    unsigned char raw_hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len = 0;
    EVP_DigestFinal_ex(context, raw_hash, &hash_len);
    EVP_MD_CTX_free(context);
    std::stringstream hex;
    for (unsigned int i = 0; i < hash_len; i++) {
      hex << std::hex << std::setw(2) << std::setfill('0')
          << static_cast<int>(raw_hash[i]);
    }
    EXPECT_EQ(hex.str(), t_case.file_hash);
  }
}
