#include "image_io_error.hpp"
#include "io/image_ver.hpp"
#include <cstddef>
#include <cstdio>
#include <expected>
#include <fcntl.h>
#include <filesystem>
#include <format>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

/*
 * Take user provided path and either add the provided file or search one level
 * deep if directory is provided.
 * Return IO image_error if path leads to non-existant file or is not directory
 * or regular file.
 */
std::expected<std::vector<fs::path>, image_error>
find_files(const fs::path path) {
  bool dir_exists = fs::exists(path);
  if (!dir_exists) {
    return std::unexpected{image_error::IO(
        0, err_severity::DEBUG,
        std::format("specified directory does not exist, path: {}",
                    path.c_str()),
        "")};
  }
  if (fs::is_directory(path)) {
    std::vector<fs::path> file_entries;
    auto iter = fs::directory_iterator{path};
    for (const auto &dir_entry : iter) {
      if (!fs::is_regular_file(dir_entry)) {
        continue;
      }
      file_entries.push_back(dir_entry);
    }
    return file_entries;
  } else if (fs::is_regular_file(path)) {
    return std::vector<fs::path>{path};
  }
  return std::unexpected<image_error>{
      image_error::IO(0, err_severity::DEBUG,
                      std::format("provided path leads to neither regular file "
                                  "or directory, path: {}",
                                  path.c_str()),
                      "")};
}

/*
 * Look through vector of paths and sort unsupported files into rejected file
 * vector to be returned with supported files remaining in the passed vector.
 * Unsupported images are return with a vector of the files and the associated
 * image_error with the reason why.
 * TODO remove jpeg and png non support once supported
 */
std::expected<std::vector<fs::path>, image_error>
sort_file(std::vector<fs::path> &files) {
  std::vector<fs::path> rej_files;
  for (size_t i = 0; i < files.size(); i++) {
    fs::path file = files.at(i);
    auto im_format_ret = image_format(file);
    if (!im_format_ret) {
      rej_files.push_back(file);
      files.erase(files.begin() + static_cast<std::ptrdiff_t>(i));
      i--;
      continue;
    }
    auto im_format = im_format_ret.value();

    if (im_format == image_type::INTER_IM) {
      auto is_raw_r = is_raw_file(file);
      if (!is_raw_r || !is_raw_r.value()) {
        rej_files.push_back(file);
        files.erase(files.begin() + static_cast<std::ptrdiff_t>(i));
        i--;
      }
      continue;
    } else if (im_format == image_type::PNG_FILE ||
               im_format == image_type::JPEG_FILE) {
      rej_files.push_back(file);
      files.erase(files.begin() + static_cast<std::ptrdiff_t>(i));
      i--;
      continue;
    } else if (im_format != image_type::TIFF_FILE) {
      rej_files.push_back(file);
      files.erase(files.begin() + static_cast<std::ptrdiff_t>(i));
      i--;
      continue;
    }
  }
  return rej_files;
}

std::expected<std::vector<char *>, image_error>
file_loader(std::vector<fs::path> file_paths,
            std::vector<std::pair<fs::path, image_error>> failed) {
  std::vector<char *> files;
  int f_desc;
  for (const auto &path : file_paths) {
    f_desc = open(path.c_str(), O_RDONLY);
    void *data;

    struct stat sbuf;
    if (stat(path.c_str(), &sbuf) == -1) {
      failed.push_back(std::make_pair(
          path, image_error::IO(0, err_severity::WARNING,
                                "stat failed to get required file info", "")));
      continue;
    }

    data = mmap(nullptr, static_cast<size_t>(sbuf.st_size), PROT_READ,
                MAP_SHARED, f_desc, 0);
    if (data == MAP_FAILED) {
      failed.push_back(std::make_pair(
          path,
          image_error::IO(0, err_severity::WARNING,
                          std::format("failed to create map for memory"), "")));
      continue;
    }
    files.push_back(static_cast<char *>(data));
  }
  return files;
}
