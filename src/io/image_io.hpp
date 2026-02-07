#pragma once

#include "image_io_error.hpp"
#include <expected>
#include <string>

enum image_type {
  RAW_FILE,
  TIFF_FILE,
  PNG_FILE,
  JPEG_FILE,
  INTER_IM,
  UNSUPPORTED_IMAGE,
  NOT_IMAGE,
  FILE_DOES_NOT_EXIST,
  FILE_EXIST,
  ERROR
};

std::expected<image_type, io_error> image_format(std::string path);
std::expected<bool, io_error> is_raw_tiff(std::string path);
bool is_raw_file(std::string path);
