//
// Created by keepersam02 on 6/22/25.
//

#include "writeTiff.h"
#include <tiffio.h>
#include <libraw/libraw.h>
#include "import.h"

int writeTiff(char* filepath, libraw_data_t* metDat, intermediateImage* rawIm, char* imData) {
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

int writeProcTiff(char* filepath, intermediateImage* img, libraw_data_t* metDat) {
    if (!filepath || !img || !metDat) {
        return -1;
    }
    TIFF* tiffIm = TIFFOpen(filepath, "w");
    if (tiffIm == NULL) {
        return -1;
    }
    //General RGB fields
    TIFFSetField(tiffIm, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(tiffIm, TIFFTAG_BITSPERSAMPLE, 16);
    TIFFSetField(tiffIm, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tiffIm, TIFFTAG_COMPRESSION, 1);
    TIFFSetField(tiffIm, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
    TIFFSetField(tiffIm, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

    TIFFSetField(tiffIm, TIFFTAG_IMAGEWIDTH, img->width);
    TIFFSetField(tiffIm, TIFFTAG_IMAGELENGTH, img->height);
    TIFFSetField(tiffIm, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tiffIm, img->width*3));

    uint16_t* deNormDat = malloc(sizeof(uint16_t) * img->dataSize);
    int res = denormalize16Bit(img->data, img->dataSize, deNormDat);
    if (res != 0) {
        free(deNormDat);
        return -1;
    }
    tsize_t lineBytes = 6*img->width;

    for (uint32_t row = 0;row < img->height; row++) {
        uint16_t* rowData = deNormDat + (row * img->width * 3);
        if (TIFFWriteScanline(tiffIm, rowData, row, 0) < 0) {
            break;
        }
    }
    free(deNormDat);
    TIFFClose(tiffIm);
    return 0;
}

int denormalize16Bit(double* data, uint dataSize, uint16_t* deNorm) {
    if (!data || !deNorm) {
        return -1;
    }
    for (int i = 0; i < dataSize;i++) {
        double val = data[i];
        if (val > 1.0) {val = 1.0;}
        if (val < 0.0) {val = 0.0;}
        deNorm[i] = (uint16_t)(val * 65535.0);
    }
    return 0;
}


