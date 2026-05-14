#include "io/image_io_error.hpp"
#include <expected>
#include <filesystem>
#include <format>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

std::expected<bool, image_error> file_copier(void *file, size_t file_size,
                                             fs::path src, fs::path dest_dir) {
  const auto file_name = src.filename();
  fs::path file_dest = dest_dir / file_name;

  auto dest_file = fopen(file_dest.c_str(), "wb");
  if (dest_file == NULL) {
    return std::unexpected(
        image_error::IO(-1, err_severity::ERROR,
                        "failed to create destination file to write to",
                        std::format("src path:{}, dest path: {}", src.c_str(),
                                    file_dest.c_str())));
  }
  auto written = fwrite(file, file_size, 1, dest_file);
  if (written == 0) {
    return std::unexpected(image_error::IO(
        -1, err_severity::ERROR, "failed to write to destination",
        std::format("src:{}, dest:{}", src.c_str(), file_dest.c_str())));
  }
  return true;
}

/**
std::vector<fs::path>
file_copy_ver(std::vector<std::pair<long, fs::path>> files) {
  for (const auto file : files) {
  }
}

std::expected<long, image_error> file_hash(void *file) {
  // use openssl to hash the source file dest
}
**/
