#include "exif.hpp"
#include "exiv2.hpp"
#include "xmp_exiv2.hpp"
#include <cstdint>
#include <stdfloat>
#include <string>
#include <vector>

class std_image {
public:
  Exiv2::ExifData exif_data;
  Exiv2::XmpData xmp_data;
}
