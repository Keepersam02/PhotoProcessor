#include "../../src/io/image_io.hpp"
#include "gtest/gtest.h"
#include <filesystem>
#include <fstream>
#include <ios>
#include <vector>

namespace fs = std::filesystem;

typedef struct {
  std::string name;
  std::string file_name;
  image_type type;
  std::array<std::byte, 16> file_signature;
} image_io_test_case;

TEST(image_io, tiff_png_valid) {

  std::array<image_io_test_case, 2> test_cases{
      image_io_test_case{
          "TIFF 1",
          "tiff_valid_1",
          TIFF_FILE,
          {std::byte(0x49), std::byte(0x49), std::byte(0x2A), std::byte(0x00)}},
      image_io_test_case{"PNG 1",
                         "png_valid_1",
                         PNG_FILE,
                         {std::byte(137), std::byte(80), std::byte(78),
                          std::byte(71), std::byte(13), std::byte(10),
                          std::byte(26), std::byte(10)}},
  };
  fs::path parent_dir = std::filesystem::temp_directory_path();
  std::vector<fs::path> files(2);

  for (int i = 0; i < test_cases.size(); i++) {
    fs::path cur = parent_dir / test_cases.at(i).file_name;
    files.push_back(cur);

    std::ofstream ostream(cur);
    ostream.write(
        reinterpret_cast<const char *>(test_cases.at(i).file_signature.data()),
        static_cast<std::streamsize>(test_cases.at(i).file_signature.size()));
    ostream.close();
  }

  for (int i = 0; i < test_cases.size(); i++) {
    image_io_test_case cur = test_cases.at(i);
    fs::path file_path = parent_dir / cur.file_name;

    image_type type = supported_image(file_path.string());
    ASSERT_EQ(type, cur.type) << "File: " << cur.file_name << std::endl;
  }
}

TEST(image_io, raw_valid) {
  std::array<image_io_test_case, 2> test_cases{
      image_io_test_case{"fuji raw",
                         "fuji_valid_1",
                         RAW_FILE,
                         {std::byte('F'), std::byte('U'), std::byte('J'),
                          std::byte('I'), std::byte('F'), std::byte('I'),
                          std::byte('L'), std::byte('M'), std::byte('C'),
                          std::byte('C'), std::byte('D'), std::byte('-'),
                          std::byte('R'), std::byte('A'), std::byte('W')}},
      image_io_test_case{"cannon raw",
                         "cannon_raw_1",
                         RAW_FILE,
                         {std::byte(0x49), std::byte(0x49), std::byte(0x2A),
                          std::byte(0x00), std::byte(0x01), std::byte(0x00),
                          std::byte(0x00), std::byte(0x00), std::byte(0x43),
                          std::byte(0x52)}}};

  fs::path parent_dir = fs::temp_directory_path();

  for (int i = 0; i < test_cases.size(); i++) {
    image_io_test_case cur = test_cases.at(i);
    std::ofstream ostream(parent_dir / cur.file_name);
    ostream.write(reinterpret_cast<const char *>(cur.file_signature.data()),
                  static_cast<std::streamsize>(cur.file_signature.size()));
    ostream.close();

    image_type type = supported_image(parent_dir / cur.file_name);
    ASSERT_EQ(type, cur.type) << "File: " << cur.file_name << std::endl;
  }
}
