//
// Created by keepersam on 9/18/25.
//

#ifndef IMAGE_PROCESSOR_TYPES_H
#define IMAGE_PROCESSOR_TYPES_H
#include <libraw_types.h>

typedef struct {
    ushort height, width;
    ushort bits, colors;
    double gamma;
    unsigned int dataSize;
    double* data;
}intermediateImage;

typedef struct {
    int size;
    int capacity;
    float* data;
} image_data_slice;

#endif //IMAGE_PROCESSOR_TYPES_H
