//
// Created by keepersam on 7/9/25.
//

#include "CSTApp.h"
#include "../file_handling/import.h"
#include "../lut-handle/lutApp.h"


int procImage(intermediateImage* img, double cstMat[9]) {
    if (!img) {
        return -1;
    }
    double* outImage = malloc(sizeof(double) * img->dataSize);
    if (!outImage) {
        return -1;
    }
    for (uint i = 0;i < img->dataSize;i+=3) {
        *(outImage + i) = clipCheck(cstMat[0] * *(img->data + i) + cstMat[1] * *(img->data + 1 + i) + cstMat[2] * *(img->data + i + 2));
        *(outImage + i + 1) = clipCheck(cstMat[3] * *(img->data + i) + cstMat[4] * *(img->data + i + 1) + cstMat[5] * *(img->data + i + 2));
        *(outImage + i + 2) = clipCheck(cstMat[6] * *(img->data + i) + cstMat[7] * *(img->data + i + 1) + cstMat[8] * *(img->data + i + 2));
    }
    free(img->data);
    img->data = outImage;
    return 0;
}