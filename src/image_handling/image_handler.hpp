#include "io/image_io_error.hpp"
#include <expected>
#include <filesystem>
namespace fs = std::filesystem;

std::expected<bool, image_error> file_copier(void *file, size_t file_size,
                                             fs::path src, fs::path dest_dir);
