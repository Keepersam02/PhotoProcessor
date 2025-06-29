//
// Created by keepersam on 6/24/25.
//

#ifndef LUTPARSE_H
#define LUTPARSE_H

int loadLUT(char* filepath, char** lutData);
int parseLUTSize(char* splDat[70], int* size);
int parseLUTTitle(char* splDat[70], char** title);
int parseLUTData65(char* lutData, float red[65], float green[65], float blue[65]);

#endif //LUTPARSE_H
