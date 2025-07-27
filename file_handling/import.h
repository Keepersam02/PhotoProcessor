//
// Created by keepersam02 on 6/21/25.
//

#ifndef IMPORT_H
#define IMPORT_H
#include "import.h"
#include "libraw/libraw.h"

typedef struct {
    ushort height, width;
    ushort bits, colors;
    double gamma;
    unsigned int dataSize;
    double* data;
}intermediateImage;

int openFile(char* path, libraw_data_t* rawReader);
bool valid_image(char* path);
int ReadImageData16(libraw_processed_image_t** img, libraw_data_t* rawProc);
int normalizeImage(char* dataIn, unsigned int size, double* normIm);
int createIntermediateImage(libraw_processed_image_t* img, void** interImP);
int normalizeImage(char* dataIn, unsigned int size, double* normIm);
int testNormalizeImage();


#endif //IMPORT_H
