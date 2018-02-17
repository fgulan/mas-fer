//
//  ppm.c
//  MAS
//
//  Created by Filip Gulan on 20/10/2017.
//  Copyright © 2017 Filip Gulan. All rights reserved.
//

#include "ppm.h"
#include <stdlib.h>

#define MAX_RGB_VALUE 255

void printErrorAndCloseStream(const char *message, FILE *fp)
{
	fprintf(stderr, "Error: %s\n", message);
	if (fp != NULL) {
		fclose(fp);
	}
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

bool checkP6FormatType(FILE *fp)
{
	// Check if it is P6 type
	if (getc(fp) != 'P' || getc(fp) != '6') {
		return false;
	}
	return true;
}

Size readSize(FILE *fp)
{
	Size size = { -1, -1 };
	fscanf(fp, "%u", &size.width);
	skipNonValidCharactersAndLines(fp);
	fscanf(fp, "%u", &size.height);
	return size;
}

int readMaxRGBValue(FILE *fp)
{
	int maxValue;
	if (fscanf(fp, "%d", &maxValue) == 1) {
		return maxValue;
	}
	return -1;
}

Image *readPPM(const char *filename)
{
	Image *image;
	int maxValue;

	// Open file handler
	FILE *fp = fopen(filename, "rb");
	if (!fp) {
		printErrorAndCloseStream(filename, fp);
		return NULL;
	}

	if (!checkP6FormatType(fp)) {
		printErrorAndCloseStream("Invalid PPM image type. Must be P6!", fp);
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
	maxValue = readMaxRGBValue(fp);
	if (maxValue <= 0 || maxValue > 65535) {
		printErrorAndCloseStream("Invalid max RGB value!", fp);
		return NULL;
	}

	// Skip single whitespace char (as defined in format)
	fseek(fp, 1, SEEK_CUR);

	// If it is everything okay until now, alloc image and underlying data
	image = (Image *)malloc(sizeof(Image));
	if (!image) {
		printErrorAndCloseStream("Unable to allocate memory for Image!", fp);
		return NULL;
	}
	// Set image size and data
	image->size = size;
	image->data = (Pixel *)malloc(size.width * size.height * sizeof(Pixel));
	if (!image->data) {
		printErrorAndCloseStream("Unable to allocate memory for Image data!", fp);
		return NULL;
	}

	size_t rd = fread(image->data, sizeof(Pixel), size.width * size.height, fp);
	if (rd < size.width * size.height) {
		printErrorAndCloseStream("Invalid image size!", fp);
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
		printErrorAndCloseStream(filename, fp);
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
