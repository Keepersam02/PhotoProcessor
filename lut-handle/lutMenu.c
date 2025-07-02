//
// Created by keepersam02 on 6/29/25.
//

#include "lutMenu.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lutParse.h"


typedef struct {
    char*   filepath;
    char*   title;
    double  data[274625];
}lut65Pt;



void generalMenu() {
    while (1) {
        printf("What would you like to do?\n1) Load LUT\n2) Save LUT (not active)\n");
        char input[10];
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;
        if (input[0] == 'b' && input[1] == '\0') {
            break;
        } else if (input[0] == '1' && input[1] == '\0') {
            void** lut = NULL;
            int lutSize = 0;
            readLUT(lut, lutSize);
            if (*lut == NULL) {
                printf("Error reading lut please try again.\n");
                continue;
            }
            if (lutSize == 65) {
                loadedLUT
            }
        }

    }
}

void readLUT(void** lut, int size) {
    while (1) {
        char input[300];
        printf("Please enter the path to the LUT you would like to load.\nOr 'b' to go back.\n");
        fgets(input, sizeof input, stdin);
        input[strcspn(input, "\n")] = 0;
        if (input[0] == 'b' && input[1] == '\0') {
            break;
        }
        printf("Searching for lut at %s\n", input);
        char* lutData;
        int res = loadLUT(input, &lutData);
        if (res != 0) {
            printf("Failed to load lut please reenter the path.\nIf you would like to exit lut load menu enter z\n");
            continue;
        }
        char *splDat[274630];
        res = splitLUTData(lutData, splDat);
        if (res != 0) {
            printf("Failed to split lut into lines.\nPlease enter the path to try again.\n");
            continue;
        }
        char* title;
        res = parseLUTTitle(splDat, &title);
        if (res != 0) {
            printf("Could not find lut title.\n");
            continue;
        }
        res = parseLUTSize(splDat, &size);
        if (res != 0) {
            printf("Failed to find lut size.\n");
            continue;
        }
        if (size == 65) {
            lut65Pt* newLut = malloc(sizeof(lut65Pt));
            if (!newLut) {
                printf("Failed to allocate memory for LUT.\n");
                break;
            }
            res = parseLUTData65(splDat, newLut->data);
            if (res != 0) {
                printf("Could not parse lut data\n");
                continue;
            }
            *lut = newLut;
            break;
        }

    }
}
