#include "exif.hpp"
#include "exiv2.hpp"
#include "xmp_exiv2.hpp"
#include <cstdint>
#include <filesystem>
#include <stdfloat>
#include <string>
#include <vector>

namespace fs = std::filesystem;

class std_image {
  std_image(fs::path path, Exiv2::ExifData exif_data, Exiv2::XmpData xmp_data,
            std::vector<float> image_data)
      : path(path), exif_data(exif_data), xmp_data(xmp_data),
        image_data(image_data) {}

public:
  fs::path path;
  Exiv2::ExifData exif_data;
  Exiv2::XmpData xmp_data;

  std::vector<float> image_data;
};
