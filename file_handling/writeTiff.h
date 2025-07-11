//
// Created by keepersam02 on 6/22/25.
//

#ifndef WRITETIFF_H
#define WRITETIFF_H
#include <libraw_types.h>
#include "import.h"

int writeTiff(char* filepath, libraw_data_t* metDat, intermediateImage* rawIm, char* imData);
int denormalize16Bit(double* data, uint dataSize, uint16_t* deNorm);
int writeProcTiff(char* filepath, intermediateImage* img, libraw_data_t* metDat);

#endif //WRITETIFF_H
