//
// Created by keepersam02 on 6/22/25.
//

#ifndef BMPWRITE_H
#define BMPWRITE_H


#pragma pack(push, 1)
typedef struct {
    unsigned short  identifier;
    unsigned long   size;
    unsigned short  reserved1;
    unsigned short  reserved2;
    unsigned long   offset;
}bmpHeader;

typedef struct {
    unsigned int   size;            // Header size in bytes (40)
    int            width;           // Width of the image
    int            height;          // Height of the image
    unsigned short planes;          // Number of color planes
    unsigned short bitCount;        // Bits per pixel
    unsigned int   compression;     // Compression type
    unsigned int   imageSize;       // Image size in bytes
    int            xPixelsPerMeter; // Pixels per meter in x axis
    int            yPixelsPerMeter; // Pixels per meter in y axis
    unsigned int   colorsUsed;      // Number of colors used
    unsigned int   colorsImportant; // Number of important colors
} BmpInfoHeader;
#pragma pack(pop)

int writeHeader(char* filepath, bmpHeader* genHead, BmpInfoHeader* infHeader);

#endif //BMPWRITE_H
