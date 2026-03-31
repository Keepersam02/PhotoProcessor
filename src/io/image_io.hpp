#include "image_io_error.hpp"
#include "io/image_ver.hpp"
#include <cstddef>
#include <expected>
#include <filesystem>
#include <format>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

std::expected<std::vector<fs::path>, image_error>
find_files(const fs::path path);

std::expected<std::vector<fs::path>, image_error>
sort_file(std::vector<fs::path> &files);

std::expected<std::vector<char *>, image_error>
file_loader(std::vector<fs::path> file_paths,
            std::vector<std::pair<fs::path, image_error>> failed);
