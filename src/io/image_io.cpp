#include "image_io.hpp"
#include "image_io_error.hpp"
#include "libraw/libraw.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <libraw/libraw_const.h>
#include <memory>
#include <optional>
#include <png.h>
#include <string>
#include <tiff.h>
#include <tiffio.h>

namespace fs = std::filesystem;

std::expected<image_type, image_error> valid_file(std::string path) {
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
std::expected<image_type, image_error> image_format(std::string path) {
  if (path.empty()) {
    return ERROR;
  }

  std::ifstream file(path);
  std::array<std::byte, 16> header{};
  file.read(reinterpret_cast<char *>(header.data()), header.size());
  if (file.gcount() != 16) {
    auto err =
        image_error::IO(1, err_severity::DEBUG, "unexpected amoount bytes read",
                        std::format("expected: 16 got: {:>10}", file.gcount()));
    file.close();
    return std::unexpected<image_error>{err};
  }

  const std::array<std::byte, 8> png_header{
      std::byte(137), std::byte(80), std::byte(78), std::byte(71),
      std::byte(13),  std::byte(10), std::byte(26), std::byte(10)};
  const std::array<std::byte, 4> tiff_header{std::byte(0x49), std::byte(0x49),
                                             std::byte(0x2A), std::byte(0x00)};
  const std::array<std::byte, 12> jpeg_header_1{
      std::byte(0xFF), std::byte(0xD8), std::byte(0xFF), std::byte(0xE0),
      std::byte(0x00), std::byte(0x10), std::byte(0x4A), std::byte(0x46),
      std::byte(0x49), std::byte(0x46), std::byte(0x00), std::byte(0x01)};
  const std::array<std::byte, 4> jpeg_header_2{
      std::byte(0xFF), std::byte(0xD8), std::byte(0xFF), std::byte(0xEE)};

  if (std::equal(tiff_header.begin(), tiff_header.end(), header.begin())) {
    return TIFF_FILE;
  }
  if (std::equal(png_header.begin(), png_header.end(), header.begin())) {
    return PNG_FILE;
  }
  if (std::equal(jpeg_header_1.begin(), jpeg_header_1.end(), header.begin()) ||
      std::equal(jpeg_header_2.begin(), jpeg_header_2.end(), header.begin())) {
    return JPEG_FILE;
  }
  return INTER_IM;
}

/*
 * Determine if a tiff file appears to be a raw file
 */
std::expected<bool, image_error> is_raw_tiff(std::string path) {
  TIFF *file = TIFFOpen(path.c_str(), "r");
  if (file == NULL) {
    auto err =
        image_error::IO_LTIFF(0, err_severity::DEBUG, "null file: " + path,
                              "occured in: is_raw_tiff");
    return std::unexpected<image_error>{err};
  }
  uint32_t count = 0;
  void *cfa_pattern = nullptr;
  uint16_t *cfa_pattern_dim = nullptr;
  uint32_t *active_field = nullptr;

  bool has_cfa = TIFFGetField(file, TIFFTAG_CFAPATTERN, &count, &cfa_pattern);
  bool has_pat_dim =
      TIFFGetField(file, TIFFTAG_CFAREPEATPATTERNDIM, &cfa_pattern_dim);
  bool has_active_field = TIFFGetField(file, TIFFTAG_ACTIVEAREA, &active_field);

  if (!has_cfa || !has_pat_dim || !has_active_field) {
    TIFFClose(file);
    return false;
  }
  TIFFClose(file);
  return true;
}

std::expected<bool, image_error> is_raw_file(std::string path) {
  LibRaw raw;
  int res = raw.open_file(path.c_str());
  if (res != 0) {
    return false;
  }
  return true;
}
