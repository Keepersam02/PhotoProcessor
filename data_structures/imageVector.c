//
// Created by keepersam on 7/7/25.
//

#include "imageVector.h"

#include <stdlib.h>


int initImgVectorDef(void** imgVector) {
    imageVector* newVector = malloc(sizeof(imageVector));
    if (!newVector) {
        printf("Buy more RAM.\n");
        return MEMINITFAIL;
    }
    intermediateImage* imageArr = malloc(sizeof(intermediateImage) * 10);
    if (!imageArr) {
        printf("Buy more RAM.\n");
        return MEMINITFAIL;
    }
    newVector->img = imageArr;
    newVector->capacity = 10;
    newVector->size = 0;
    *imgVector = newVector;
    return SUCCESS;
}

int initImgVector(void** imgVector, uint capacity) {
    imageVector* newVector = malloc(sizeof(imageVector));
    if (!newVector) {
        printf("Buy more RAM.\n");
        return MEMINITFAIL;
    }
    intermediateImage* imageArr = malloc(sizeof(intermediateImage) * capacity);
    if (!imageArr) {
        printf("Buy more RAM.\n");
        return MEMINITFAIL;
    }
    newVector->img = imageArr;
    newVector->capacity = capacity;
    newVector->size = 0;
    *imgVector = newVector;
    return SUCCESS;
}

int expandImgVector(imageVector* imageVec) {
    if (!imageVec) {
        return NULLPTR;
    }
    uint newCap = imageVec->capacity * 2;
    intermediateImage* newArr = malloc(sizeof(intermediateImage) * newCap);
    if (!newArr) {
        printf("Buy more RAM.\n");
        return -1;
    }
    for (int i = 0;i < imageVec->capacity;i++) {
        *(newArr + i) = *(imageVec->img + i);
    }
    free(imageVec->img);
    imageVec->img = newArr;
    imageVec->capacity = newCap;

    return 0;
}

int pushImgVector(imageVector* imageVec, intermediateImage* newImage) {
    if (!imageVec) {
        return NULLPTR;
    }
    if (imageVec->size == imageVec->capacity) {
        int res = expandImgVector(imageVec);
        if (res != 0) {
            return RESIZEFAIL;
        }
    }
    *(imageVec->img + imageVec->size) = *newImage;
    imageVec->size++;
    return SUCCESS;
}

int popImgVect(imageVector* imageVec, intermediateImage* img) {
    if (!imageVec) {
        return NULLPTR;
    }
    if (imageVec->size == 0) {
        return -1;
    }
    *img = *(imageVec->img + imageVec->size - 1);
    imageVec->size--;
    return SUCCESS;
}

int insertImgVec(imageVector* imgVec, intermediateImage* img, uint index) {
    if (!imgVec || !img) {
        return NULLPTR;
    }
    if (imgVec->capacity == imgVec->size) {
        int res = expandImgVector(imgVec);
        if (res != 0) {
            return RESIZEFAIL;
        }
    }
    while (imgVec->capacity <= index) {
        int res = expandImgVector(imgVec);
        if (res != 0) {
            return RESIZEFAIL;
        }
    }
    for (uint i = imgVec->size - 1;i > index; i--) {
        *(imgVec->img + i + 1) = *(imgVec->img + i);
    }
    *(imgVec->img + index) = *(img);

    return 0;
}

int removeImgVec(imageVector* imgVec, uint index) {
    if (!imgVec) {
        return NULLPTR;
    }
    if (index >= imgVec->size) {
        return SUCCESS;
    }
    for (uint i = index;index <= imgVec->size;i++) {
        *(imgVec->img + i) = *(imgVec->img + i + 1);
    }
    imgVec->size--;
    return SUCCESS;
}

int imVecToString(imageVector* imgVec, char string[1000]) {
    if (!imgVec) {
        return NULLPTR;
    }


    for (int i = 0; i < imgVec->size;i++) {
        char interImCur[100];
        sprintf(interImCur, "Hieght: %d, Width: %d\nBits: %d, Colors: %i, Gamma: %f\nData size: %i\n", imgVec->img[i].height, imgVec->img[i].width
            ,imgVec->img[i].bits, imgVec->img[i].colors, imgVec->img[i].gamma, imgVec->img[i].dataSize);
        strcat(string, interImCur);
    }
    return SUCCESS;
}
