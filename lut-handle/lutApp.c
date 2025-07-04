//
// Created by keepersam on 6/24/25.
//

#include "lutApp.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double clipCheck(double in) {
    if (in > 1) {
        return 1;
    }
    if (in < 0) {
        return 0;
    }
    return in;
}

int applyPix65(double in[3], double lutDat[274625]) {
    int rIndex = (int)(in[0] * 64.0f);
    int gIndex = (int)(in[1] * 64.0f);
    int bIndex = (int)(in[2] * 64.0f);

    int index = rIndex + (gIndex * 65) + (bIndex * 65 * 65);
    if (index > 274625) {
        printf("Exeeded maximum index for 65pt lut.\n");
        return -1;
    }

    in[0] = clipCheck(lutDat[index]);
    in[1] = clipCheck(lutDat[index + 1]);
    in[2] = clipCheck(lutDat[index + 2]);

    return 0;
}

int applyIm65(double* data, int size, double lutDat[274625]) {
    double* processed = malloc(size * sizeof(double));
    if (!data) {
        printf("Null array\n");
        return -1;
    }
    int i = 0;
    int res;
    for (i; i < size; i++) {
        double in[3] = {*(data + i), *(data + i + 1), *(data + i + 2)};
        res = applyPix65(in, lutDat);
        if (res != 0) {
            free(processed);
            printf("Failed to apply pixel %i\n", i);
            return -1;
        }
        *(processed + i) = in[0];
        *(processed + i + 1) = in[1];
        *(processed + i + 2) = in[2];
    }
    data = processed;
    free(processed);
    return 0;
}