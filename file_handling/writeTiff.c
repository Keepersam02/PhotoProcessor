//
// Created by keepersam02 on 6/22/25.
//

#include "writeTiff.h"
#include <tiffio.h>
#include <libraw/libraw.h>

int writeTiff(char* filepath, libraw_data_t* metDat, libraw_processed_image_t* rawIm, char* imData) {
    TIFF* file = TIFFOpen(filepath, "w");
    if (file == NULL) {
        return -1;
    }
    TIFFSetField(file, TIFFTAG_IMAGEWIDTH, rawIm->width);
    TIFFSetField(file, TIFFTAG_IMAGELENGTH, rawIm->height);
    TIFFSetField(file, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(file, TIFFTAG_BITSPERSAMPLE, rawIm->bits);
    TIFFSetField(file, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(file, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(file, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    tsize_t linebytes = 3*rawIm->width;
    if (rawIm->bits == 16) {
        linebytes = linebytes*2;
    }
    unsigned char *buf = NULL;

    if (TIFFScanlineSize(file) == linebytes) {
        buf = (unsigned char*)_TIFFmalloc(linebytes);
    } else {
        buf = (unsigned char*)_TIFFmalloc(TIFFScanlineSize(file));
    }
    if (!buf) {
        return -1;
    }
    TIFFSetField(file, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(file, rawIm->width*3));

    unsigned char* imDat = rawIm->data;
    for (uint32_t row = 0; row < rawIm->height;row++) {
        uint32_t srcRow = rawIm->height - row - 1;
        memcpy(buf, imDat + (srcRow * linebytes), linebytes);
        if (TIFFWriteScanline(file, buf, row, 0) < 0) {
            break;
        }
    }
    TIFFClose(file);
    _TIFFfree(buf);
    return 0;
}


