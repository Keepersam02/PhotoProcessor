#include "convert.hpp"
#include "exiv2/exiv2.hpp"
#include "image.hpp"
#include "io/image_io.hpp"
#include "io/image_io_error.hpp"
#include "io/image_ver.hpp"
#include "libraw.h"
#include "types.hpp"
#include "types/image.hpp"
#include "xmp_exiv2.hpp"
#include <cstddef>
#include <cstdint>
#include <expected>
#include <tuple>
#include <utility>
#include <vector>

namespace fs = std::filesystem;

/*
 * General image handler.  Takes a set of file poths and a pipeline and executes
 * the pipeline on the imaeges.
 *   - determine file type
 *   - load into memory
 *   - debayer, pre-debayered support later
 *   - execute pipeline
 *   - pass to appropriate export controller
 */
std::expected<std::vector<std_image>, image_error>
handler(std::vector<fs::path> file_paths, std::vector<int *> pipeline,
        fs::path output_dir) {
  std::vector<std::pair<fs::path, image_error>> failed;
  auto in_mem_files = file_loader(file_paths, failed);
  if (!in_mem_files) {
    // todo
  }
  if (failed.size() != 0) {
  }
  failed.clear();
  auto &images = image_creator(in_mem_files, failed);
  if (!images) {
    // todo
  }
  for (auto &image : images) {
    for (const auto func : pipeline) {
      auto ret = func(image);
      if (!ret) {
        // todo
      }
      image = ret;
    }
  }
  return images;
}

std::expected<std::vector<std::tuple<std::vector<float>, exiv2::ExifData,
                                     Exiv2::XmpData>>,
              image_error>
image_creator(std::vector<std::pair<void *, size_t>> files,
              std::vector<std::pair<char *, image_error>> failed) {
  std::vector<std::tuple<std::vector<flaot>, Exiv2::ExifData, Exiv2::XmpData>>
      images;
  for (const auto &file : files) {
    auto image_dat = get_image_data(file.first, file.second);
    if (!image_dat) {
      return image_error::IO(0, err_severity::ERROR,
                             "failed to get image data from file",
                             "fail to debayer")
    }
    auto exif_data = get_exif_data(file.first, file.second);
    if (!exif_data) {
      image_error::IO(0, err_severity::ERROR,
                      "failed to retrieve data from file",
                      "exif data retrieval faileure");
    }
    images.push_back({image_dat, exif_data.first, exif_data.second})
  }
  return images;
}

std::expected<std::vector<float>, image_error> get_image_data(const void *file,
                                                              size_t size) {
  LibRaw i_proc;
  i_proc.open_buffer(file.first, file.second);
  i_proc.unpack();
  i_proc.imgdata.params.output_bps = 16;
  i_proc.imgdata.params.output_color = 8;
  i_proc.imgdata.params.gamm[0] = 1.0;
  i_proc.imgdata.params.gamm[1] = 1.0;
  i_proc.imgdata.params.no_auto_bright = 1;
  i_proc.dcraw_process();
  std::vector<float> image_data;
  auto *data = reinterpret_cast<uint16_t *>(i_proc.imgdata.image);
  for (int i = 0;
       i < i_proc.imgdata.sizes.height * i_proc.imgdata.sizes.width * 4;
       i += 4) {
    image_data.push_back(data[i] / 65535);
    image_data.push_back(data[i + 1] / 65535);
    image_data.push_back(data[i + 2] / 65535);
  }
  return image_data;
}

std::expected<std::pair<Exiv2::ExifData, Exiv2::XmpData>, image_error>
get_exif_data(void *file, size_t size) {
  auto *file_p = static_cast<const Exiv2::byte *>(file);
  auto exif_p = Exiv2::ImageFactory::open(file_p, size);
  exif_p->readMetatdata();

  auto const exif_meta_data = exif_p->exifData();
  auto const xmp_data = exif_p->expData();
  return {exif_meta_data, xmp_data};
}
