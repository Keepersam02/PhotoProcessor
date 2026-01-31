#pragma once

#include <string>

enum image_type {
  RAW_FILE,
  TIFF_FILE,
  PNG_FILE,
  JPEG_FILE,
  UNSUPPORTED_IMAGE,
  NOT_IMAGE,
  FILE_DOES_NOT_EXIST,
  FILE_EXIST,
  ERROR
};

image_type image_format(std::string path);
image_type is_raw_tiff(std::string path);
