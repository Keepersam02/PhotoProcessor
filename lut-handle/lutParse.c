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

int splitLUTData(char* lutData, char *splDat[], int *max_lines) {
    char* og = strdup(lutData);
    char* datCpy = og;
    if (!datCpy) {
        return -1;
    }
    char* curLine = strtok(datCpy, "\r\n");
    int current_max_lines = *max_lines;
    int line_number = 0;
    while (curLine != NULL) {
        if (line_number >= current_max_lines) {
            current_max_lines *= 2;

            char** temp = realloc(splDat, current_max_lines * sizeof(char*));
            if (!temp) {
                free(og);
                return -1;
            }
            splDat = temp;
            *max_lines = current_max_lines;
        }
        splDat[line_number] = strdup(curLine);
        curLine = strtok(NULL, "\r\n");
        line_number++;
    }
    free(og);
    return line_number;
}

int parse_lut(char *raw_data[], void** parsed_lut, int* size) {
    lut_65_pt* parsLut = malloc(sizeof(lut_65_pt));
    if (!raw_data || !size) {
        return -1;
    }
    int cur_line_num = 0;
    char *cur_line = NULL;
    while (cur_line == NULL || cur_line_num >= *size) {
        cur_line = strstr(raw_data[cur_line_num], "TITLE");
        cur_line_num++;
    }
    if (cur_line_num >= *size) {
        free(parsLut);
        return -2;
    }
    char* lutTitle = NULL;
    *lutTitle = *cur_line;
    parsLut->lut_title = cur_line;


}

int string_match(char* str_1, char* str_2) {

}
