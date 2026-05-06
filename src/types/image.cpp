#include "image.hpp"
#include "io/image_io_error.hpp"
#include <exiv2/exif.hpp>
#include <exiv2/exiv2.hpp>
#include <exiv2/xmp_exiv2.hpp>
#include <expected>
#include <memory>
#include <utility>

std::expected<std::pair<Exiv2::ExifData, Exiv2::XmpData>, image_error>
get_exif_data(const void *file, size_t size) {
  auto *file_p = static_cast<const Exiv2::byte *>(file);
  auto exif_p = Exiv2::ImageFactory::open(file_p, static_cast<long>(size));
  exif_p->readMetadata();

  auto exif_meta_data = exif_p->exifData();
  auto xmp_data = exif_p->xmpData();
  return std::pair<Exiv2::ExifData, Exiv2::XmpData>{exif_meta_data, xmp_data};
}

struct std_image::Exiv2Context {
  Exiv2::ExifData exif_data_;
  Exiv2::XmpData xmp_data_;

  Exiv2Context(const void *file, size_t size) {
    auto exif_info = get_exif_data(file, size);
    if (!exif_info) {
      // todo
    } else {
      exif_data_ = exif_info->first;
      xmp_data_ = exif_info->second;
    }
  }
};

std_image::std_image(fs::path path, std::vector<float> im_data,
                     const void *file, size_t size)
    : pimpl_(std::make_unique<Exiv2Context>(file, size)),
      path_(std::move(path)), image_data_(std::move(im_data)) {}
std_image::~std_image() = default;
