#include "image_io_error.hpp"
#include <cstddef>
#include <expected>
#include <filesystem>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

std::expected<std::vector<fs::path>, image_error>
find_files(const fs::path path);

std::expected<std::vector<std::pair<void *, size_t>>, image_error>
file_loader(std::vector<fs::path> file_paths,
            std::vector<std::pair<fs::path, image_error>> &failed);

std::expected<std::pair<void *, size_t>, image_error>
load_file(fs::path file_path);
