#include <cstdint>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

struct exif_data {};

class image_meta {
  fs::path image_path_;
  exif_data exif_data_;

  uint64_t get_id() { return image_id_; }

  image_meta(fs::path image_path) { image_path_ = image_path; }

private:
  uint64_t image_id_;

  exif_data create_exif_data(fs::path image_path);
};

class image_meta_group {
  std::vector<image_meta> images_;

  image_meta_group(size_t num_images) {
    images_.reserve(num_images);
    next_id_ = 0;
  }

  uint64_t insert_image(image_meta &new_image) {
    images_.insert(images_.begin() + static_cast<long>(next_id_), new_image);
    next_id_++;
    return next_id_ - 1;
  }

private:
  uint64_t next_id_;
};
