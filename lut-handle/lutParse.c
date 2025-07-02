//
// Created by keepersam on 6/24/25.
//

#include <stdio.h>
#include <stdlib.h>
#include "lutParse.h"

#include <inttypes.h>
#include <string.h>



int loadLUT(char* filepath, char** lutData) {
    if (!filepath) {
        printf("Invalid filepath pointer\n");
        return -1;
    }
    FILE* file = fopen(filepath, "rb");
    if (!file) {
        printf("Error reading lut file.\n");
        return -1;
    }
    fseek(file, 0, SEEK_END);
    long fSize = ftell(file);
    fseek(file, 0, SEEK_SET);


    *lutData = malloc(fSize);
    if (!*lutData) {
        printf("Could not allocate mem for lut\n");
        return -1;
    }
    fread(*lutData, fSize, 1, file);
    fclose(file);
    return 0;
}

int splitLUTData(char* lutData, char *splDat[274630]) {
    char* og = strdup(lutData);
    char* datCpy = og;
    if (!datCpy) {
        return -1;
    }
    char* curLine = strtok(datCpy, "\r\n");
    int i = 0;
    while (curLine != NULL) {
        splDat[i] = strdup(curLine);
        curLine = strtok(NULL, "\r\n");
        i++;
        if (i >= 274630) {
            free(og);
            printf("LUT file too long.\n Only enter 65 point or smaller LUTS\n");
            return -1;
        }
    }
    free(og);
    return 0;
}

int parseLUTTitle(char *splDat[274630], char** title) {
    char* og = strdup(splDat[0]);
    char* titleStr = og;
    titleStr = strtok(titleStr, "\"");
    if (titleStr == NULL) {
        printf("Could not find title.");
        return -1;
    }
    titleStr = strtok(NULL, "\"");
    if (titleStr == NULL) {
        printf("Could not find title\n");
        return -1;
    }
    *title = titleStr;
    free(og);
    return 0;
}

int parseLUTSize(char *splDat[274630], int* size) {
    char* og = strdup(splDat[1]);
    char* token = og;
    token = strtok(token, " ");
    token = strtok(NULL, " ");

    *size = strtol(token, NULL, 10);
    free(og);
    return 0;
}

int parseLUTData65(char *splDat[274630], double data[274625]) {
    char* og;
    int datIn = 0;
    double tempD;
    for (int i = 2;i < 274627;i++) {
        og = strdup(splDat[i]);
        char* temp = og;
        temp = strtok(og, " ");
        tempD = atof(temp);
        data[datIn] = tempD;
        datIn++;
        temp = strtok(NULL, " ");
        tempD = atof(temp);
        data[datIn] = tempD;
        datIn++;
        temp = strtok(NULL, " ");
        tempD = atof(temp);
        data[datIn] = tempD;
        datIn++;
        free(og);
    }
    printf("%i\n", datIn);
    return 0;
}