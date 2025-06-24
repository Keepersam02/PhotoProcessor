//
// Created by keepersam02 on 6/22/25.
//

#include "bmpWrite.h"

#include <stdio.h>

/*int writeHeader(char* filepath, bmpHeader* genHead, BmpInfoHeader* infHeader) {
    if (filepath == NULL || genHead == NULL || infHeader == NULL) {
        printf("Passes null pointer to write bmp header");
        return 0;
    }
    FILE* newBmp = fopen(filepath, "wb");
    size_t written = fwrite(genHead, sizeof bmpHeader, 1, newBmp);
    if (written != sizeof bmpHeader) {
        printf("Error writing bmp header.");
        return -1;
    }
    written = fwrite(infHeader, sizeof BmpInfoHeader, 1, newBmp);
    if (written != sizeof BmpInfoHeader) {
        printf("Error writing info header");
        return -1;
    }
    fclose(newBmp);
    return 0;
}*/
