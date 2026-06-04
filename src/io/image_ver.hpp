#pragma once

#include "image_io_error.hpp"
#include <expected>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

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

std::expected<std::vector<fs::path>, image_error>
sort_file(std::vector<fs::path> &files);
std::expected<image_type, image_error> image_format(const fs::path &path);
std::expected<bool, image_error> is_raw_tiff(const fs::path &path);
std::expected<bool, image_error> is_raw_file(const fs::path &path);
