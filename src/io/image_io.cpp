#include "image_handling/file_pool.hpp"
#include "image_io_error.hpp"
#include "tiff.h"
#include "types/image.hpp"
#include "types/image_files.hpp"
#include "types/image_internal.hpp"
#include "types/image_types.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exiv2/exif.hpp>
#include <exiv2/exiv2.hpp>
#include <expected>
#include <fcntl.h>
#include <filesystem>
#include <format>
#include <fstream>
#include <ios>
#include <liburing.h>
#include <liburing/io_uring.h>
#include <ostream>
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

bool import_images_uring(image_files &images, file_pool &pool) {
  struct io_uring ring;
  auto res = io_uring_queue_init(24, &ring, 0);
  if (res != 0) {
    // err
    return false;
  }
  unsigned in_flight = 0;
  uint64_t num_comp = 0;
  while (num_comp < images.num_files) {

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    while (in_flight < 24 && (sqe = io_uring_get_sqe(&ring)) != nullptr) {
      auto index = images.get_not_loaded();
      if (index == UINT64_MAX) {
        break;
      }
      auto im = &images.files[index];
      io_uring_sqe_set_data64(sqe, im->get_id());
      int fd = open(im->file_path_.c_str(), O_RDONLY);
      im->fd = fd;

      struct stat sbuf;
      fstat(fd, &sbuf);

      auto slot = pool.get_slot();
      im->file_data = slot;

      io_uring_prep_read(sqe, fd, slot, static_cast<unsigned>(sbuf.st_size), 0);
      in_flight++;
    }
    if (in_flight > 0) {
      io_uring_submit(&ring);
    }
    struct io_uring_cqe *cqe;
    if (in_flight > 0) {
      if (io_uring_wait_cqe(&ring, &cqe) == 0) {
        struct io_uring_cqe *cqes[24];
        unsigned count = 24;
        unsigned num_filled = io_uring_peek_batch_cqe(&ring, cqes, count);
        for (unsigned i = 0; i < num_filled; i++) {
          auto cur = cqes[i];
          images.ins_loaded(cur->user_data);
          pool.slot_ready(images.files[cur->user_data].file_data);
          close(images.files[cur->user_data].fd);
          num_comp++;
        }
        io_uring_cq_advance(&ring, num_filled);
        in_flight -= num_filled;
      }
    }
  }
  return true;
}

bool import_images_sys(image_files &images, file_pool &pool) {

  bool all_smooth = true;
  auto index = images.get_not_loaded();
  while ((index = images.get_not_loaded()) != 0) {
    auto im = &images.files[index];
    int fd = open(im->file_path_.c_str(), O_RDONLY);

    struct stat statbuf;
    int ret = fstat(fd, &statbuf);
    if (ret == -1) {
      all_smooth = false;
      images.ins_failed(im->get_id());
      continue;
    }

    void *buf = pool.get_slot();
    ssize_t num_read = read(fd, buf, static_cast<unsigned>(statbuf.st_size));
    if (num_read != statbuf.st_size) {
      all_smooth = false;
      images.ins_failed(im->get_id());
      pool.free_slot(buf);
      index = images.get_not_loaded();
      continue;
    }
    images.files[index].file_data = buf;
    images.ins_loaded(im->get_id());
    pool.slot_ready(buf);
  }
  return all_smooth;
}
bool import_images_std(image_files &images, file_pool &pool) {

  bool all_smooth = true;
  uint64_t index;
  while ((index = images.get_not_loaded()) != 0) {
    std::ifstream i_stream;
    auto &im = images.files[index];

    i_stream.open(im.file_path_, std::ios_base::binary | std::ios_base::in);
    i_stream.seekg(std::ios_base::end);
    auto file_size = i_stream.tellg();
    i_stream.seekg(std::ios_base::beg);

    auto buffer = pool.get_slot();
    i_stream.read(static_cast<char *>(buffer), file_size);
    if (!i_stream) {
      if (i_stream.bad()) {
      } else if (i_stream.fail() && !i_stream.eof()) {
      } else if (i_stream.eof()) {
      }
      all_smooth = false;
      images.ins_failed(im.get_id());
      pool.free_slot(buffer);
      continue;
    }
    if (i_stream.gcount() != file_size) {
      all_smooth = false;
      images.ins_failed(im.get_id());
      pool.free_slot(buffer);
      continue;
    }

    images.files[index].file_data = buffer;
    images.ins_loaded(im.get_id());
    pool.slot_ready(buffer);
  }
  return all_smooth;
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
