#include "io/image_io_error.hpp"
#include "types/image.hpp"
#include <exif.hpp>
#include <expected>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

std::expected<std::vector<std_image>, image_error>
handler(std::vector<fs::path> file_paths, std::vector<int *> pipeline);

std::expected<std::vector<std_image>, image_error>
image_creator(std::vector<std::pair<void *, size_t>> files,
              std::vector<std::pair<char *, image_error>> failed);

std::expected<std::pair<Exiv2::ExifData, Exiv2::XmpData>, image_error>
get_exif_data(void *file, size_t size);
