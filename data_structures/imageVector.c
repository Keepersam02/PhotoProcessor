//
// Created by keepersam on 7/7/25.
//

#include "imageVector.h"

#include <stdlib.h>

int initImgVectorDef(void** imgVector) {
    imageVector* newVector = malloc(sizeof(imageVector));
    if (!newVector) {
        printf("Buy more RAM.\n");
        return -1;
    }
    intermediateImage* imageArr = malloc(sizeof(intermediateImage) * 10);
    if (!imageArr) {
        printf("Buy more RAM.\n");
        return -1;
    }
    newVector->img = imageArr;
    newVector->capacity = 10;
    newVector->size = 0;
    *imgVector = newVector;
    return 0;
}

int initImgVector(void** imgVector, uint capacity) {
    imageVector* newVector = malloc(sizeof(imageVector));
    if (!newVector) {
        printf("Buy more RAM.\n");
        return -1;
    }
    intermediateImage* imageArr = malloc(sizeof(intermediateImage) * capacity);
    if (!imageArr) {
        printf("Buy more RAM.\n");
        return -1;
    }
    newVector->img = imageArr;
    newVector->capacity = capacity;
    newVector->size = 0;
    return 0;
}

int expandImgVector(imageVector* imageVec) {
    uint newCap = imageVec->capacity * 2;
    intermediateImage* newArr = malloc(sizeof(intermediateImage) * newCap);
    for (int i = 0;i < imageVec->ca) {

    }
}
