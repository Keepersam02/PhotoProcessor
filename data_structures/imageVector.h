//
// Created by keepersam on 7/7/25.
//

#ifndef IMAGEVECTOR_H
#define IMAGEVECTOR_H
#include <libraw_types.h>

typedef struct {
    ushort height, width;
    ushort bits, colors, gamma;
    unsigned int dataSize;
    double* data;
}intermediateImage;

typedef struct {
    uint size;
    uint capacity;
    intermediateImage* img;
}imageVector;

#endif //IMAGEVECTOR_H
