//
// Created by keepersam02 on 6/21/25.
//

#ifndef IMPORT_H
#define IMPORT_H
#include "import.h"
#include "libraw/libraw.h"

int openFile(char* path, libraw_data_t* rawReader);
bool valid_image(char* path);
int ReadImageData16(libraw_processed_image_t** img, libraw_data_t* rawProc);

#endif //IMPORT_H
