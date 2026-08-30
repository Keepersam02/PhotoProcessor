#include "image_handling/file_pool.hpp"
#include "image_io_error.hpp"
#include "types/image_files.hpp"
#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

struct path_id {
  fs::path f_path;
  uint64_t f_id;
};

std::expected<bool, image_error>
import_images_uring(file_pool &pool, std::vector<path_id> &f_paths,
                    image_files &images);

bool import_images_std(image_files &images, fs::path error_out);

std::expected<std::vector<fs::path>, image_error>
find_files(const fs::path path);

std::expected<std::vector<std::pair<void *, size_t>>, image_error>
file_loader(std::vector<fs::path> file_paths,
            std::vector<std::pair<fs::path, image_error>> &failed);

std::expected<std::pair<void *, size_t>, image_error>
load_file(const fs::path &file_path);
