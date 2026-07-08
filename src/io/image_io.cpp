#include "image_io.hpp"
#include "image_adjustments/file_pool.hpp"
#include "image_handling/image_handler.hpp"
#include "image_io_error.hpp"
#include "tiff.h"
#include "types/image.hpp"
#include "types/image_internal.hpp"
#include <cstddef>
#include <cstdio>
#include <exiv2/exif.hpp>
#include <exiv2/exiv2.hpp>
#include <expected>
#include <fcntl.h>
#include <filesystem>
#include <format>
#include <liburing.h>
#include <liburing/io_uring.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <system_error>
#include <tiffio.h>
#include <unistd.h>
#include <utility>
#include <vector>

const u_int IO_URING_DEPTH = 16;

namespace fs = std::filesystem;

std::expected<bool, image_error> import_images(file_pool pool,
                                               std::vector<path_id> f_paths) {
  std::vector<path_id> in_progess;
  struct io_uring ring;
  int ret = io_uring_queue_init(IO_URING_DEPTH, &ring, 0);
  if (ret < 0) {
    return std::unexpected<image_error>(image_error::IO(
        ret, err_severity::SEVERE, std::generic_category().message(errno),
        "failed to initialize io_uring"));
  }

  while (f_paths.empty() && in_progess.empty()) {
    auto sqe = io_uring_get_sqe(&ring);
    if (!sqe) {
      continue;
    }
    auto slot = pool.get_slot();
    const auto next = f_paths.back();
    int fd = open(next.f_path.c_str(), O_RDONLY);
    struct stat f_stat;
    ret = fstat(fd, &f_stat);
    io_uring_prep_read(sqe, fd, slot, static_cast<unsigned>(f_stat.st_size), 0);
  }
}

/*
 * Take user provided path and either add the provided file or search one
 * level deep if is provided. Return IO image_error if path leads to
 * non-existant file or is not directory or regular file.
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
std::expected<std::vector<std::pair<void *, size_t>>, image_error>
file_loader(std::vector<fs::path> file_paths,
            std::vector<std::pair<fs::path, image_error>> &failed) {
  std::vector<std::pair<void *, size_t>> files;
  for (const auto &path : file_paths) {
    const auto res = load_file(path);
    if (!res) {
      failed.push_back(std::make_pair(path, res.error()));
      continue;
    }
    files.push_back(res.value());
  }
  return files;
}

std::expected<std::pair<void *, size_t>, image_error>
load_file(const fs::path &file_path) {
  auto f_desc = open(file_path.c_str(), O_RDONLY);
  if (f_desc == -1) {
    std::string err = std::system_category().message(errno);
    return std::unexpected<image_error>(image_error::IO(
        -1, err_severity::ERROR,
        std::format("Failed to open {}", file_path.c_str()), err));
  }

  struct stat sbuf;
  {
    auto ret = stat(file_path.c_str(), &sbuf);
    if (ret == -1) {
      std::string err = std::system_category().message(errno);
      close(f_desc);
      return std::unexpected<image_error>(image_error::IO(
          -1, err_severity::ERROR,
          std::format("Failed to get stat for file: {}", file_path.c_str()),
          err));
    }
  }
  void *data;
  data = mmap(nullptr, static_cast<size_t>(sbuf.st_size), PROT_READ, MAP_SHARED,
              f_desc, 0);
  if (data == MAP_FAILED) {
    std::string err = std::system_category().message(errno);
    close(f_desc);
    return std::unexpected<image_error>(image_error::IO(
        -1, err_severity::ERROR,
        std::format("Failed to map file: {}", file_path.c_str()), err));
  }
  close(f_desc);
  return std::make_pair(data, static_cast<size_t>(sbuf.st_size));
}

std::expected<bool, image_error> tiff_exporter(fs::path out_dir, int suffix,
                                               std_image image) {
  Exiv2::ExifData &raw_exif = get_internal_context(image).exif_data_;

  auto file_name = image.path_.filename();
  std::string name = file_name.c_str();
  name.append(std::format("{}", suffix));
  auto output_path = fs::path(out_dir / file_name);
  output_path.replace_extension(".tiff");
  TIFF *file = TIFFOpen(output_path.c_str(), "w");
  TIFFSetField(file, TIFFTAG_IMAGEWIDTH, raw_exif["Exif.Image.ImageWidth"]);
  TIFFSetField(file, TIFFTAG_IMAGELENGTH, raw_exif["Exif.Image.ImageLength"]);
  TIFFSetField(file, TIFFTAG_SAMPLESPERPIXEL,
               raw_exif["Exif.Image.SamplesPerPixel"]);
  TIFFSetField(file, TIFFTAG_BITSPERSAMPLE,
               raw_exif["Exif.Image.BitsPerSample"]);
  TIFFSetField(file, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
  TIFFSetField(file, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
  TIFFSetField(file, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  return true;
}
