//
// Created by keepersam on 7/7/25.
//

#ifndef IMAGEVECTOR_H
#define IMAGEVECTOR_H
#include <libraw_types.h>
#include "../file_handling/import.h"

typedef enum {
    SUCCESS = 0,
    MEMINITFAIL = -1,
    RESIZEFAIL = -2,
    NULLPTR = -3,
}vecOperationResults;

typedef struct {
    uint size;
    uint capacity;
    intermediateImage* img;
}imageVector;

int initImgVectorDef(void** imgVector);
int initImgVector(void** imgVector, uint capacity);
int expandImgVector(imageVector* imageVec);
int pushImgVector(imageVector* imageVec, intermediateImage* newImage);
int popImgVect(imageVector* imageVec, intermediateImage* img);
int insertImgVec(imageVector* imgVec, intermediateImage* img, uint index);
int removeImgVec(imageVector* imgVec, uint index);
int imVecToString(imageVector* imgVec, char string[1000]);
#endif //IMAGEVECTOR_H
