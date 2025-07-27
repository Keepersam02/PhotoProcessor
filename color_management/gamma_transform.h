//
// Created by keepersam on 7/14/25.
//

#ifndef GAMMA_TRANSFORM_H
#define GAMMA_TRANSFORM_H

#include "../file_handling/import.h"
#include <math.h>

int gamma_shift(intermediateImage* img, double power_shift);
int target_gamma(intermediateImage* img, double target_gamma);
int davinci_inter_forward(intermediateImage* img);
int davinci_inter_inverse(intermediateImage* img);

#endif //GAMMA_TRANSFORM_H
