#pragma once
#include "exiv2/exiv2.hpp"
#include "image.hpp"
#include <exiv2/xmp_exiv2.hpp>

struct std_image::Exiv2Context {
  Exiv2::ExifData exif_data_;
  Exiv2::XmpData xmp_data_;

  Exiv2Context(const void *file, size_t size);
};

inline std_image::Exiv2Context &get_internal_context(std_image &image) {
  return *image.pimpl_;
}
