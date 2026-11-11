#include "proc.hpp"
#include "libraw.h"
#include <cstdio>
#include <unistd.h>

extern char main_slab[2000000000];

int debayer_write(char *out_path, void *file, size_t file_size) {
  LibRaw raw_proc(0);

  {
    int ret = raw_proc.open_buffer(static_cast<char *>(file), file_size);
    if (ret != 0) {
      fprintf(stderr, "failed to open buffer in libraw, err: %s, path: %s\n",
              LibRaw::strerror(ret), out_path);
      return ret;
    }
    printf("opened buffer for out: %s\n", out_path);
  }
  // raw_proc.imgdata.params.gamm[0] = 4.5f;
  // raw_proc.imgdata.params.gamm[1] = 1/2.222;
  raw_proc.imgdata.params.output_color = 1;

  raw_proc.imgdata.params.output_bps = 16;
  raw_proc.imgdata.params.user_qual = 3;

  raw_proc.unpack();
  raw_proc.dcraw_process();

  raw_proc.dcraw_ppm_tiff_writer(out_path);
  return 0;
}
