#include "libraw/libraw.h"
#include <array>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

bool valid_file(std::string path) {
  fs::path file_path(path);
  if (!fs::exists(file_path)) {
    return false;
  }

  std::array<std::byte, 16> magic_num{};
  std::ifstream i_file(file_path, std::ios::binary);
  i_file.read(reinterpret_cast<char *>(magic_num.data()), magic_num.size());
  if (i_file.gcount() != 16) {
    std::cerr << "failed to read 16 bytes from " << file_path.filename()
              << " read " << i_file.gcount() << std::endl;
    i_file.close();
    return false;
  }
  i_file.close();

  std::array<std::byte, 10> cr2{
      std::byte(0x49), std::byte(0x49), std::byte(0x2A), std::byte(0x00),
      std::byte(0x10), std::byte(0x00), std::byte(0x00), std::byte(0x00),
      std::byte(0x43), std::byte(0x52)};
  std::array<std::byte, 16> fraw{
      std::byte{'F'}, std::byte{'U'}, std::byte{'J'}, std::byte{'I'},
      std::byte{'F'}, std::byte{'I'}, std::byte{'L'}, std::byte{'M'},
      std::byte{'C'}, std::byte{'C'}, std::byte{'D'}, std::byte{'-'},
      std::byte{'R'}, std::byte{'A'}, std::byte{'W'}};

  std::array<std::array<std::byte, 10>, 1> length_10{cr2};
  std::array<std::array<std::byte, 16>, 1> length_16{fraw};

  for (std::array<std::byte, 10> cur : length_10) {
    if (std::equal(magic_num.begin(), magic_num.begin() + 10, cur.begin())) {
      return true;
    }
  }
  for (std::array<std::byte, 16> cur : length_16) {
    if (std::equal(magic_num.begin(), magic_num.end(), cur.begin())) {
      return true;
    }
  }
  return false;
}
