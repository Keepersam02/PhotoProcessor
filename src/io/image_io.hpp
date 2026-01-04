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
  FILE_EXIST
};

image_type valid_file(std::string path);
image_type supported_image(std::string path);
