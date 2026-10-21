#include "image_pre_proc.hpp"
#include "file_pool.hpp"
#include "libraw.h"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>

void debayer_images(image_files &files, file_pool &pool, image_tiles &tiles,
                    tile_pool &t_pool, struct debayer_params *params) {
  while ((files.get_failed_size() + files.get_done_size()) != files.num_files) {
    void *slot = pool.get_ready();
    uint64_t file_id = *static_cast<uint64_t *>(slot);

    image_file file = files.files[file_id];
  }
}

int debayer_im(image_file &file, void *slot, struct debayer_params *params,
               tile_pool &t_pool) {
  LibRaw raw_proc(0);

  int res = raw_proc.open_buffer(
      static_cast<void *>(static_cast<uint64_t *>(slot) + 1),
      file.raw_file_size);
  if (res != 0) {
  }
  raw_proc.unpack();

  raw_proc.imgdata.params.gamm[0] = params->tar_gamma[0];
  raw_proc.imgdata.params.gamm[1] = params->tar_gamma[1];
  raw_proc.imgdata.params.output_color = static_cast<int>(params->tar_c_space);
  raw_proc.imgdata.params.output_bps = 16;

  res = raw_proc.dcraw_process();
  if (res != 0) {
  }

  file.im_info.c_space = params->tar_c_space;
  file.im_info.gamma[0] = params->tar_gamma[0];
  file.im_info.gamma[1] = params->tar_gamma[1];
}

void image_tiler(image_file &file, float *slot, image_tiles &tiles,
                 size_t tile_width, tile_pool &t_pool) {
  im_info info = file.im_info;
  size_t num_data_pts = info.height * info.width * info.channels;

  size_t num_wide = info.width / tile_width;
  size_t rem_wide = info.width % tile_width;

  size_t num_height = info.height / tile_width;
  size_t rem_hieght = info.height / tile_width;
}

void create_tile(float *im_data, size_t tile_width, tile_pool &pool,
                 image_tile &tile) {
  void *raw_slot = static_cast<float *>(pool.get_free_slot());

  memcpy(raw_slot, &tile, sizeof(image_tile));

  float *slot = static_cast<float *>(
      static_cast<void *>(static_cast<image_tile *>(raw_slot) + 1));
  for (size_t i = 0; i < tile_width; i++) {
    for (size_t j = 0; j < tile_width; j += 4) {
      *(slot + i * tile_width + j) = *(im_data + i * tile_width + j);
      *(slot + i * tile_width + j + 1) = *(im_data + i * tile_width + j + 1);
      *(slot + i * tile_width + j + 2) = *(im_data + i * tile_width + j + 2);
      *(slot + i * tile_width + j + 3) = *(im_data + i * tile_width + j + 3);
    }
  }
}

void create_partial_tile(float *im_data, size_t tile_width, image_tile &tile,
                         tile_pool &pool)
