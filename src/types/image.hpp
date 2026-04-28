#include <cstdint>
#include <filesystem>
#include <memory>
#include <stdfloat>
#include <string>
#include <vector>
#pragma once

namespace fs = std::filesystem;

class std_image {

private:
  struct Exiv2Context;
  std::unique_ptr<Exiv2Context> pimpl_;

  friend Exiv2Context &get_internal_context(std_image &image);

public:
  fs::path path_;
  std::vector<float> image_data_;

  std_image(fs::path path, std::vector<float> im_data, const void *file,
            size_t size);
  ~std_image();
};
