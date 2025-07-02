//
// Created by keepersam on 6/24/25.
//

#ifndef LUTPARSE_H
#define LUTPARSE_H

int loadLUT(char* filepath, char** lutData);
int splitLUTData(char* lutData, char *splDat[274630]);
int parseLUTTitle(char *splDat[274630], char** title);
int parseLUTSize(char *splDat[274630], int* size);
int parseLUTData65(char *splDat[274630], double data[274625]);

#endif //LUTPARSE_H
