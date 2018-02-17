//
//  pgm.c
//  MAS_DZ2
//
//  Created by Filip Gulan on 06/01/2018.
//  Copyright © 2018 Filip Gulan. All rights reserved.
//

#include "pgm.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define PGM_READ_BUFFER_LEN 256
#define MAX_RGB_VALUE 65536

void printErrorAndCloseStream(const char *message, FILE *fp)
{
    fprintf(stderr, "Error: %s\n", message);
    fclose(fp);
}

void skipCommentLines(FILE *fp)
{
    char c;
    c = getc(fp);
    while (c == '#') {
        // Read until new line, which signalise new comment or real data after that
        while (getc(fp) != '\n');
        // Read next line start character
        c = getc(fp);
    }
    // We have read all the comments and started reading new line with data so return last read character
    ungetc(c, fp);
}

void skipWhitespaces(FILE *fp)
{
    char c;
    do {
        c = getc(fp);
    } while (isspace(c));
    // We have read all whitespace and started reading new data so return last read character
    ungetc(c, fp);
}

void skipNonValidCharactersAndLines(FILE *fp)
{
    skipWhitespaces(fp);
    skipCommentLines(fp);
    skipWhitespaces(fp);
}

bool checkP5FormatType(FILE *fp)
{
    // Check if it is P6 type
    if (getc(fp) != 'P' || getc(fp) != '5') {
        return false;
    }
    return true;
}

Size readSize(FILE *fp)
{
    Size size = {-1, -1};
    fscanf(fp, "%u", &size.width);
    skipNonValidCharactersAndLines(fp);
    fscanf(fp, "%u", &size.height);
    return size;
}

int readMaxGreyValue(FILE *fp)
{
    int maxGreyValue;
    if (fscanf(fp, "%d", &maxGreyValue) == 1) {
        return maxGreyValue;
    }
    return -1;
}

PGMImage *readPGM(const char *filename)
{
    PGMImage *image;
    int maxGreyValue;
    bool is2BytePixel;
    size_t pixelCount;
    
    // Open file handler
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printErrorAndCloseStream(filename, fp);
        return NULL;
    }
    
    if (!checkP5FormatType(fp)) {
        printErrorAndCloseStream("Invalid PGM image type. Must be P5!", fp);
        return NULL;
    }
    
    // Skip all whitespaces and comments
    skipNonValidCharactersAndLines(fp);
    
    // Read size
    Size size = readSize(fp);
    if (size.width <= 0 || size.height <= 0) {
        printErrorAndCloseStream("Invalid image size!", fp);
        return NULL;
    }
    
    // Skip all whitespaces and comments
    skipNonValidCharactersAndLines(fp);
    
    // Check if maximum RGB value is valid
    maxGreyValue = readMaxGreyValue(fp);
    if (maxGreyValue <= 0 || maxGreyValue > 65535) {
        printErrorAndCloseStream("Invalid max RGB value!", fp);
        return NULL;
    }
    // Determine pixel size
    is2BytePixel = maxGreyValue > 255;
    
    // Skip single whitespace char (as defined in format)
    fseek(fp, 1, SEEK_CUR);
    
    // If it is everything okay until now, alloc image and underlying data
    image = (PGMImage *)malloc(sizeof(PGMImage));
    if (!image) {
        printErrorAndCloseStream("Unable to allocate memory for Image!", fp);
        return NULL;
    }
    // Set image size and data
    image->size = size;
    image->data_1 = NULL;
    image->data_2 = NULL;
    
    // Alloc memory for image pixels
    if (is2BytePixel) {
        image->data_2 = (PGMPixel_2 *)malloc(size.width * size.height * sizeof(PGMPixel_2));
    } else {
        image->data_1 = (PGMPixel_1 *)malloc(size.width * size.height * sizeof(PGMPixel_1));
    }
    
    if (!image->data_1 && !image->data_2) {
        printErrorAndCloseStream("Unable to allocate memory for Image data!", fp);
        return NULL;
    }
    
    // Read pixels
    if (is2BytePixel) {
        pixelCount = fread(image->data_2, sizeof(PGMPixel_2), size.width * size.height, fp);
    } else {
        pixelCount = fread(image->data_1, sizeof(PGMPixel_1), size.width * size.height, fp);
    }
    
    if (pixelCount < size.width * size.height) {
        printErrorAndCloseStream("Invalid image size!", fp);
        return NULL;
    }
    
    fclose(fp);
    return image;
}

void freePGMImage(PGMImage *pgmImage)
{
    if (pgmImage == NULL) {
        return;
    }
    if (pgmImage->data_1 != NULL) {
        free(pgmImage->data_1);
    }
    if (pgmImage->data_2 != NULL) {
        free(pgmImage->data_2);
    }
    free(pgmImage);
}
