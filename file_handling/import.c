//
// Created by keepersam02 on 6/21/25.
//

#include "import.h"
#include "libraw/libraw.h"
#include <sys/stat.h>
#include <libraw/libraw_version.h>

typedef enum {
    DIRECTORY = 0,
    REGULARFILE = 1,

}fileTypes;

int fileType(char* path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0) {
        return -1;
    }
    int res = S_ISDIR(statbuf.st_mode);
    if (S_ISDIR(statbuf.st_mode) == 0) {
        return DIRECTORY;
    }
    if (S_ISREG(statbuf.st_mode) == 0) {
        return REGULARFILE;
    }
    return -2;
}

int openDir(char* path, )

int openFile(char* path, libraw_data_t* rawReader) {
    int res = libraw_open_file(rawReader, path);
    if (res != 0) {
        return -1;
    }
    return 0;
}

bool valid_image(char* path) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        fclose(file);
        return false;
    }
    printf("File exists\n");

    char buffer[16];
    size_t bytesRead = fread(buffer, 1, 16, file);
    buffer[15] = '\0';
    fclose(file);
    if (bytesRead != 16) {
        printf("Could not read image header");
        return 0;
    }
    char magicNum[16] = {'F', 'U', 'J', 'I', 'F', 'I', 'L', 'M', 'C', 'C', 'D', '-', 'R', 'A', 'W', '\0'};
    printf("%s\n", buffer);
    printf("%s\n", magicNum);
    for (int i = 0;i < 15;i++) {
        if (buffer[i] != magicNum[i]) {
            printf("Non matching at index %i\n", i);
            return false;
        }
    }
    return true;
}

int ReadImageData16(libraw_processed_image_t** img, libraw_data_t* rawProc) {
    rawProc->params.gamm[0] = 1;
    rawProc->params.gamm[1] = 1;

    rawProc->params.use_camera_wb = 1;
    rawProc->params.use_fuji_rotate = 1;
    rawProc->params.output_bps = 16;
    rawProc->params.user_qual = 10;
    rawProc->params.no_auto_bright = 1;
    rawProc->params.output_color = 5;

    int res = libraw_unpack(rawProc);
    if (res != 0) {
        printf("Problem processing image.");
        return -1;
    }
    libraw_dcraw_process(rawProc);
    *img = libraw_dcraw_make_mem_image(rawProc, &res);
    return 0;
}

int createIntermediateImage(libraw_processed_image_t* img, void** interImP) {
    if (!img) {
        return -1;
    }
    intermediateImage* interIm = malloc(sizeof(intermediateImage));
    interIm->bits = img->bits;
    interIm->colors = img->colors;
    interIm->height = img->height;
    interIm->width = img->width;
    interIm->dataSize = img->data_size;
    interIm->gamma = 1;
    double* normData = malloc(sizeof(double) * interIm->dataSize);
    int res = normalizeImage(img->data, img->data_size, normData);
    if (res != 0) {
        free(normData);
        free(interIm);
        return -1;
    }
    interIm->data = normData;
    *interImP = interIm;
    return 0;
}

int normalizeImage(char* dataIn, unsigned int size, double* normIm) {
    if (!dataIn || !normIm) {
        return -1;
    }
    uint16_t cur;
    for (int i = 0; i < size; i++) {
        memcpy(&cur, dataIn + (i * 2), sizeof(uint16_t));
        *(normIm + i) = (double)cur / 65535;
    }
    return 0;
}

