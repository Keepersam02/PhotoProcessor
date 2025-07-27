//
// Created by keepersam on 7/14/25.
//

#include "gamma_transform.h"
#include "../file_handling/import.h"
#include <math.h>

int gamma_shift(intermediateImage* img, double power_shift) {
    if (!img) {
        return -1;
    }
    for (uint i = 0;i < img->dataSize; i++) {
        *(img->data + i) = pow(*(img->data + i), power_shift);
    }
    img->gamma = img->gamma * power_shift;
    return 0;
}

int target_gamma(intermediateImage* img, double target_gamma) {
    if (!img) {
        return -1;
    }
    double gamut_adjust = target_gamma / img->gamma;
    for (uint i = 0; i < img->dataSize;i++) {
        *(img->data + i) = pow(*(img->data + i), gamut_adjust);
    }
    img->gamma = target_gamma;
    return 0;
}

int davinci_inter_forward(intermediateImage* img) {
    if (!img) {
        return -1;
    }
    double DI_A = 0.0075;
    double DI_B = 7.0;
    double DI_C = 0.07329248;
    double DI_M = 10.44426855;
    double DI_LIN_CUT = 0.00262409;
    for (int i = 0; i < img->dataSize; i++) {
        double upper_val = DI_C * (log2(*(img->data + i) + DI_A) + DI_B);
        double lower_val = DI_M * *(img->data + i);
        if (*(img->data) > DI_LIN_CUT) {
            *(img->data + i) = upper_val;
        } else if (*(img->data) <= DI_LIN_CUT) {
            *(img->data + i) = lower_val;
        }
    }
    return 0;
}

int davinci_inter_inverse(intermediateImage* img) {
    if (!img) {
        return -1;
    }
    double DI_A = 0.0075;
    double DI_B = 7.0;
    double DI_C = 0.07329248;
    double DI_M = 10.44426855;
    double DI_LOG_CUT = 0.02740668;

    for (int i = 0;i < img->dataSize; i++) {
        double upper_val = pow(2.0, (*(img->data + i) / DI_C) - DI_B) - DI_A;
        double lower_val = *(img->data + i) / DI_M;

        if (*(img->data + i) > DI_LOG_CUT) {
            *(img->data + i) = upper_val;
        } else if (*(img->data + i) <= DI_LOG_CUT) {
            *(img->data + i) = lower_val;
        }
    }
    return 0;
}
