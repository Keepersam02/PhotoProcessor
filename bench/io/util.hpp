#ifndef UTIL
#define UTIL

#include "image_handling/file_pool.hpp"
#include "types/image_files.hpp"
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;

std::unique_ptr<image_files> create_image_files(fs::path test_dir,
                                                unsigned &largest_size);

void file_pool_plucker(file_pool &pool, image_files &images);
#endif
