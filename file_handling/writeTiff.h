//
// Created by keepersam02 on 6/22/25.
//

#ifndef WRITETIFF_H
#define WRITETIFF_H
#include <libraw_types.h>

int writeTiff(char* filepath, libraw_data_t* metDat, libraw_processed_image_t* rawIm, char* imData);

#endif //WRITETIFF_H
