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
#include <tiff.h>
#include <tiffio.h>

namespace fs = std::filesystem;

namespace {
bool is_raw(std::string path);
}

typedef struct {
  int length;
  std::array<std::byte, 16> data{};
} file_signature;

image_type valid_file(std::string path) {
  fs::path file_path(path);
  if (!fs::exists(file_path) || !fs::is_regular_file(path)) {
    return FILE_DOES_NOT_EXIST;
  }
  return FILE_EXIST;
}

/*
 *  Determine if an image is a TIFF, PNG, or JPEG file.
 *  If it is TIFF pass it to a secondary fuction to check for tags indicating a
 * raw file. IF it is png or JPEG return appropriate If it is none then check if
 * libraw can read it and return appropriately
 */
image_type image_format(std::string path) {
  if (path.empty()) {
    return ERROR;
  }

  std::ifstream file(path);
  std::array<std::byte, 16> header{};
  file.read(reinterpret_cast<char *>(header.data()), header.size());
  if (file.gcount() != 16) {
    std::cerr << "failed to read expected number of bytes from: " << path
              << " read: " << file.gcount() << std::endl;
    file.close();
  }

  const std::array<std::byte, 8> png_header{
      std::byte(137), std::byte(80), std::byte(78), std::byte(71),
      std::byte(13),  std::byte(10), std::byte(26), std::byte(10)};
  const std::array<std::byte, 4> tiff_header{std::byte(0x49), std::byte(0x49),
                                             std::byte(0x2A), std::byte(0x00)};

  bool is_png =
      std::equal(png_header.begin(), png_header.end(), header.begin());
  bool is_tiff =
      std::equal(tiff_header.begin(), tiff_header.end(), header.begin());

  if (is_tiff) {
    return RAW_FILE;
  }
  if (is_png) {
    return PNG_FILE;
  }
  if (is_tiff) {
    return TIFF_FILE;
  }
  return NOT_IMAGE;
}

/*
 * Determine if a tiff file appears to be a raw file
 */
bool is_raw_tiff(std::string path) {
  TIFF *file = TIFFOpen(path.c_str(), "r");
  if (file == NULL) {
    return false;
  }
  char cfa_pattern;
  char cfa_pattern_dim;
  char active_field = 0;

  if (TIFFGetField(file, TIFFTAG_CFAPATTERN, &cfa_pattern) != 1 &&
      TIFFGetField(file, TIFFTAG_CFAREPEATPATTERNDIM, &cfa_pattern_dim) != 1 &&
      TIFFGetField(file, TIFFTAG_ACTIVEAREA, active_field) != 1) {
    return false;
  }
  return true;
}
