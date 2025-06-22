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

int splitLUTData(char* lutData, char* splDat[70]) {
    char* datCpy = strdup(lutData);
    if (!datCpy) {
        return -1;
    }
    char* curLine = strtok(datCpy, "\r\n");
    int i = 0;
    while (curLine != NULL) {
        splDat[i] = curLine;
        curLine = strtok(datCpy, "\r\n");
        i++;
        if (i >= 70) {
            printf("LUT file too long.\n Only enter 65 point or smaller LUTS\n");
            return -1;
        }
    }
    return 0;
}

int parseLUTTitle(char* splDat[70], char** title) {
    char* titleStr = NULL;
    strcpy(titleStr, splDat[0]);
    titleStr = strtok(titleStr, "\"");
    if (titleStr == NULL) {
        printf("Could not find title.");
        return -1;
    }
    titleStr = strtok(NULL, "\"");
    if (titleStr== NULL) {
        printf("Could not find title\n");
        return -1;
    }
    *title = titleStr;
    return 0;
}

int parseLUTSize(char* splDat[70], int* size) {
    char* sizeStr = splDat[1];
    char curChar = *sizeStr;
    int i = 0;
    while (1) {
        if (curChar == '\0' || curChar == '\n') {
            printf("Could not find lut size\n");
            return -1;
        }
        char* endptr;
        long int sizeL = strtol((sizeStr + i), &endptr, 10);
        if (*endptr != curChar) {
            *size = sizeL;
            return 0;
        }
        i++;
        curChar = *(sizeStr + i);
    }
}

int parseLUTData65(char* lutData, float red[65], float green[65], float blue[65]) {
    strtok(lutData, "\r\n");
    return 0;
}