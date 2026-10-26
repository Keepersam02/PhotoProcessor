#include "file_pool.hpp"
#include "types/image_types.hpp"

struct debayer_params {
  color_space tar_c_space;
  float tar_gamma[2];
};

void debayer_images(image_files &files, file_pool &pool,
                    struct debayer_params *params);
