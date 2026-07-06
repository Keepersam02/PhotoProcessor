#include "io/image_io_error.hpp"
#include <expected>
#include <filesystem>
#include <vector>
namespace fs = std::filesystem;

const int NUM_FILE_POOLS = 5;

struct files_info {
  size_t max_size;
  size_t average_size;
  size_t threshholds[NUM_FILE_POOLS];
};

std::expected<bool, image_error> file_copier(void *file, size_t file_size,
                                             fs::path src, fs::path dest_dir);
bool file_copy_ver(std::vector<std::pair<fs::path, fs::path>> &files,
                   std::vector<std::pair<fs::path, fs::path>> &failed);
std::expected<files_info, image_error>
get_image_stats(const std::vector<fs::path> &file_paths);
