#include "image_io.hpp"
#include "libraw/libraw.h"
#include <array>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <libraw/libraw_const.h>
#include <memory>
#include <png.h>
#include <string>
#include <tiffio.h>

namespace fs = std::filesystem;

image_type valid_file(std::string path) {
  fs::path file_path(path);
  if (!fs::exists(file_path) || !fs::is_regular_file(path)) {
    return FILE_DOES_NOT_EXIST;
  }
  return FILE_EXIST;
}

image_type supported_image(std::string path) {
  LibRaw raw;
  if (raw.open_file(path.c_str()) != LIBRAW_SUCCESS) {
    return RAW_FILE;
  }
  raw.recycle();

  std::ifstream file(path);
  std::array<std::byte, 16> header{};
  file.read(reinterpret_cast<char *>(header.data()), header.size());
  if (file.gcount() != 16) {
    std::cerr << "failed to read expected number of bytes from: " << path
              << " read: " << file.gcount() << std::endl;
    file.close();
  }

  std::array<std::byte, 8> png_header{
      std::byte(137), std::byte(80), std::byte(78), std::byte(71),
      std::byte(13),  std::byte(10), std::byte(26), std::byte(10)};
  std::array<std::byte, 4> tiff_header{std::byte(0x49), std::byte(0x49),
                                       std::byte(0x2A), std::byte(0x00)};

  bool is_png =
      std::equal(png_header.begin(), png_header.end(), header.begin());
  bool is_tiff =
      std::equal(tiff_header.begin(), tiff_header.end(), header.begin());
  if (is_png) {
    return PNG_FILE;
  }
  if (is_tiff) {
    return TIFF_FILE;
  }
  return UNSUPPORTED_IMAGE;
}
