#include "proc.hpp"
#include "libraw.h"

int debayer_write(char *out_path, void *file, size_t file_size) {
  LibRaw raw_proc(0);

  {
    int ret = raw_proc.open_buffer(static_cast<char *>(file) + 751, file_size);
    if (ret != 0) {
      return ret;
    }
  }
  raw_proc.imgdata.params.gamm[0] = 4.5f;
  raw_proc.imgdata.params.gamm[1] = 1 / 2.222;
  raw_proc.imgdata.params.output_color = 1;

  raw_proc.imgdata.params.output_bps = 16;

  raw_proc.unpack();
  raw_proc.dcraw_process();

  raw_proc.dcraw_ppm_tiff_writer(out_path);
  return 0;
}
