#include "../../src/io/image_io.hpp"
#include "../../src/io/image_io_error.hpp"
#include "../../src/io/image_ver.hpp"
#include "openssl/evp.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <ios>
#include <tiff.h>
#include <tiffio.h>
#include <vector>

namespace fs = std::filesystem;

namespace {
struct test_file {
  std::string name;
  fs::path file_path;
  bool is_dir;
  bool is_reg;
  bool is_valid;
  bool is_nested;
  size_t file_size;
  std::string file_hash;
  // todo add more for stuff like a file hash...
};

struct io_test_case {
  std::string name;
  int num_succ_nest;
  int num_rej_nest;
  int num_succ_no_nest;
  int num_rej_no_nest;
  int num_file_no_nest;
  std::vector<test_file> files;
};
} // namespace

class ImageIOFixture : public testing::Test {
protected:
  fs::path master_dir;
  std::vector<io_test_case> test_cases;

  void SetUp() override {
    master_dir = fs::temp_directory_path() / "bpp_test";
    fs::create_directory(master_dir);
    std::vector<io_test_case> t_cases{
        {"no_nest_all_valid",
         2,
         0,
         2,
         0,
         2,
         {{"_DSF7242.RAF",
           fs::path(master_dir) / "no_nest_all_valid" / "_DSF7242.RAF", false,
           true, true, false, 58881856, "e61c04bf3275756a403d372e6fecca5e"},
          {"_DSF7226.RAF",
           fs::path(master_dir) / "no_nest_all_valid" / "_DSF7226.RAF", false,
           true, true, false, 59019072, "54b39742b464fe9af609d304a17f7ea6"}}},
        {"no_nest_no_valid",
         0,
         2,
         0,
         2,
         2,
         {{"rand.txt", fs::path(master_dir) / "no_nest_no_valid" / "rand.txt",
           false, true, false, false},
          {"rand2.txt", fs::path(master_dir) / "no_nest_no_valid" / "rand2.txt",
           false, true, false, false}}},
        {"no_nest_mix",
         2,
         1,
         2,
         1,
         3,
         {{"rand.txt", fs::path(master_dir) / "no_nest_mix" / "rand.txt", false,
           true, false, false},
          {"_DSF7226.RAF",
           fs::path(master_dir) / "no_nest_mix" / "_DSF7226.RAF", false, true,
           true, false, 59019072, "54b39742b464fe9af609d304a17f7ea6"},
          {"_DSF7242.RAF",
           fs::path(master_dir) / "no_nest_mix" / "_DSF7242.RAF", false, true,
           true, false, 58881856, "e61c04bf3275756a403d372e6fecca5e"}}}};

    for (const io_test_case t_case : t_cases) {
      fs::path test_path = fs::path(master_dir) / t_case.name;
      if (!fs::create_directory(test_path)) {
        std::cout << "failed to create test directory: " << t_case.name
                  << std::endl;
        continue;
      }
      for (const test_file file : t_case.files) {
        fs::path file_path = fs::path(test_path) / file.name;
        if (file.is_dir) {
          fs::create_directory(file_path);
          continue;
        } else if (!file.is_valid && file.is_reg) {
          std::ofstream ostream(file_path, std::ios::out);
          std::string message = "in a bottle, some bs to make this long enough "
                                "to read properly so it doesnt return weirdly";
          ostream.write(message.data(), message.size());
        } else if (file.is_valid) {
          CopyImage(file.name, file_path);
        }
      }
    }
    test_cases = t_cases;
  }

  void TearDown() override { fs::remove_all(master_dir); }

private:
  bool CopyImage(std::string name, fs::path dest) {
    fs::path image_src = fs::path(TEST_IMAGE_DIR) / name;
    std::ifstream istream(image_src, std::ios::binary);
    istream.seekg(0, std::ios::end);
    size_t file_size = istream.tellg();
    EXPECT_NE(file_size, -1) << "read directory as file: " << dest.string();
    istream.seekg(std::ios::beg);

    char *buffer = new char[file_size];
    istream.read(buffer, file_size);
    if (file_size != istream.tellg()) {
      std::cout << "failed to read test image: " << image_src.string()
                << std::endl;
      delete[] buffer;
      return false;
    }

    std::ofstream ostream(dest, std::ios::binary);
    ostream.write(buffer, file_size);
    if (ostream.tellp() != file_size) {
      std::cout << "failed to write test file: " << dest.string() << std::endl;
      delete[] buffer;
      return false;
    }
    delete[] buffer;
    return true;
  }
};

TEST_F(ImageIOFixture, image_io_find_files) {
  for (const io_test_case &t_case : test_cases) {
    fs::path test_path = fs::path(master_dir) / t_case.name;
    auto ret = find_files(test_path);
    ASSERT_EQ(ret.has_value(), true)
        << "expected return value doesnt exist, test name: "
        << t_case.name; // todo change after updating test struct

    auto value = ret.value();
    ASSERT_EQ(value.size(), t_case.num_file_no_nest)
        << "expected size does not match ret, test: " << t_case.name
        << " wanted: " << t_case.files.size();

    for (const auto &file : t_case.files) {
      if (file.is_dir || file.is_reg && !file.is_nested) {
        ASSERT_THAT(value, ::testing::Contains(file.file_path));
      }
    }
  }
}

TEST_F(ImageIOFixture, image_io_sort_file) {
  for (auto const &t_case : test_cases) {
    std::vector<fs::path> file_paths;
    for (auto const &file : t_case.files) {
      file_paths.push_back(file.file_path);
    }
    auto res = sort_file(file_paths);
    ASSERT_EQ(res.has_value(), true);
    auto val = res.value();
    ASSERT_EQ(val.size(), t_case.num_rej_nest) << t_case.name;
    ASSERT_EQ(file_paths.size(), t_case.num_succ_nest) << t_case.name;
  }
}

namespace {
std::string get_hash(const void *file, size_t size);
}

TEST_F(ImageIOFixture, file_loader) {
  for (auto const &t_case : test_cases) {
    std::vector<fs::path> file_paths;
    std::vector<test_file> test_files;
    for (auto const &file : t_case.files) {
      if (file.is_valid) {
        file_paths.push_back(file.file_path);
        test_files.push_back(file);
      }
    }
    std::vector<std::pair<fs::path, image_error>> failed;
    auto res = file_loader(file_paths, failed);
    EXPECT_EQ(res.has_value(), true);

    auto const res_vec = res.value();
    EXPECT_EQ(res_vec.size(), t_case.num_succ_no_nest) << t_case.name;
    ASSERT_EQ(test_files.size(), res_vec.size())
        << "expected result vector size does not match actual, expected: "
        << test_files.size() << ", got: " << res_vec.size();

    for (int i = 0; i < test_files.size(); i++) {
      const auto test_file = test_files.at(i);
      const auto loaded_pair = res_vec.at(i);
      auto file_hash = get_hash(loaded_pair.first, loaded_pair.second);
      EXPECT_EQ(test_file.file_size, loaded_pair.second)
          << "file sizes do not match, file: " << test_file.name
          << " expected: " << test_file.file_size
          << " got: " << loaded_pair.second;
      EXPECT_EQ(file_hash, test_file.file_hash)
          << "file hashes do not match, file: " << test_file.name
          << " expected: " << test_file.file_hash << " got: " << file_hash;
    }
  }
}

namespace {
std::string get_hash(const void *file, size_t size) {
  EVP_MD_CTX *context = EVP_MD_CTX_new();
  const EVP_MD *md = EVP_md5();
  unsigned char raw_hash[EVP_MAX_MD_SIZE];
  unsigned int hash_length = 0;

  EVP_DigestInit_ex(context, md, nullptr);
  EVP_DigestUpdate(context, file, size);
  EVP_DigestFinal_ex(context, raw_hash, &hash_length);
  EVP_MD_CTX_free(context);

  std::stringstream hex;
  for (unsigned int i = 0; i < hash_length; i++) {
    hex << std::hex << std::setw(2) << std::setfill('0')
        << static_cast<int>(raw_hash[i]);
  }
  return hex.str();
}
} // namespace
