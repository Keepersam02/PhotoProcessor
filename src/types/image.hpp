#include <cstdint>
#include <exiv2/exiv2.hpp>
#include <filesystem>
#include <stdfloat>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class std_image {
  std_image(fs::path path, Exiv2::ExifData exif_data, Exiv2::XmpData xmp_data,
            std::vector<float> image_data)
      : path_(path), exif_data_(exif_data), xmp_data_(xmp_data),
        image_data_(image_data) {}

public:
  fs::path path_;
  Exiv2::ExifData exif_data_;
  Exiv2::XmpData xmp_data_;

  std::vector<float> image_data_;
};
