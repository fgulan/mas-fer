//
//  ppm.c
//  MAS
//
//  Created by Filip Gulan on 20/10/2017.
//  Copyright © 2017 Filip Gulan. All rights reserved.
//

#include "ppm.h"
#include <stdlib.h>

#define BUFFER_SIZE 16
#define MAX_RGB_VALUE 255

#pragma mark - Private helpers

void printError(const char *message)
{
    fprintf(stderr, "Error: %s", message);
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

Size readSize(FILE *fp)
{
    Size size = {-1, -1};
    fscanf(fp, "%d %d", &size.width, &size.height);
    return size;
}

bool isMaxRGBValueValid(FILE *fp)
{
    int maxRGBValue;
    bool isValid = false;
    if (fscanf(fp, "%d", &maxRGBValue) == 1) {
        isValid = maxRGBValue == MAX_RGB_VALUE;
    }
    return isValid;
}

#pragma mark - Interface

Image *readPPM(const char *filename)
{
    Image *image;
    char buffer[BUFFER_SIZE];
    // Open file handler
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        printError(filename);
        return NULL;
    }
    
    // Read first line where PPM format is stored
    if (!fgets(buffer, sizeof(buffer), fp)) {
        printError(filename);
        return NULL;
    }
    
    // Check if it is P6 type
    if (buffer[0] != 'P' || buffer[1] != '6') {
        printError("Invalid PPM image type. Must be P6!");
        return NULL;
    }
    
    // Skip all comments
    skipCommentLines(fp);
    
    // Read size
    Size size = readSize(fp);
    if (size.width <= 0 || size.height <= 0) {
        printError("Invalid image size!");
        return NULL;
    }
    
    // Check if maximum RGB value is valid
    if (!isMaxRGBValueValid(fp)) {
        printError("Invalid max RGB value!");
        return NULL;
    }
    
    // Skip all chars until new line
    fseek(fp, 1, SEEK_CUR);
    
    // If it is everything okay until now, alloc image and underlying data
    image = (Image *)malloc(sizeof(Image));
    if (!image) {
        printError("Unable to allocate memory for Image!");
        return NULL;
    }
    // Set image size
    image->size = size;
    
    // Alloc memory for image pixels
    image->data = (Pixel *)malloc(size.width * size.height * sizeof(Pixel));
    if (!image->data) {
        printError("Unable to allocate memory for Image data!");
        return NULL;
    }
    
    // Read pixels (R G B R G B...)
    //             (R G B R G B...)
    size_t rd = fread(image->data, sizeof(Pixel), size.width * size.height, fp);
    if (rd < size.width * size.height) {
        printError("Invalid image size!");
        return NULL;
    }
    
    fclose(fp);
    return image;
}

bool writePPM(const char *filename, Image *image)
{
    FILE *fp;
    // Open File handler
    fp = fopen(filename, "wb");
    if (!fp) {
        printError(filename);
        return false;
    }
    
    // Write image file
    // Image format
    fprintf(fp, "P6\n");
    // Image size
    fprintf(fp, "%d %d\n", image->size.width, image->size.height);
    // Max RGB value
    fprintf(fp, "%d\n", MAX_RGB_VALUE);
    // Image pixels
    fwrite(image->data, 3 * image->size.width, image->size.height, fp);
    
    fclose(fp);
    return true;
}
