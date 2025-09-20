//
// Created by keepersam on 7/7/25.
//

#ifndef IMAGEVECTOR_H
#define IMAGEVECTOR_H
#include <libraw_types.h>
#include "../file_handling/import.h"
#include "../photo_processor_types.h"


int initImgVectorDef(void** imgVector);
int initImgVector(void** imgVector, uint capacity);
int expandImgVector(imageVector* imageVec);
int pushImgVector(imageVector* imageVec, intermediate_image* newImage);
int popImgVect(imageVector* imageVec, intermediate_image* img);
int insertImgVec(imageVector* imgVec, intermediate_image* img, uint index);
int removeImgVec(imageVector* imgVec, uint index);
int imVecToString(imageVector* imgVec, char string[1000]);
#endif //IMAGEVECTOR_H
