#include "io/image_io_error.hpp"
#include <expected>
#include <filesystem>
#include <vector>
namespace fs = std::filesystem;

std::expected<bool, image_error> file_copier(void *file, size_t file_size,
                                             fs::path src, fs::path dest_dir);

bool file_copy_ver(std::vector<std::pair<fs::path, fs::path>> &files,
                   std::vector<std::pair<fs::path, fs::path>> &failed);
