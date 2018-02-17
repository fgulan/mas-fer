//
//  decoder.c
//  MAS
//
//  Created by Filip Gulan on 25/10/2017.
//  Copyright © 2017 Filip Gulan. All rights reserved.
//

#include <stdio.h>
#include "ppm.h"
#include "jfif.h"

void encode(char *filename);
void decode();

int main(int argc, const char * argv[])
{
	encode(argv[1]);
	//decode();
	return 0;
}

void encode(char *filename)
{
	Image *image = readPPM(filename);
	ImageYCC *yccImage = rgbImageToYCCImage(image);
	ImageYCC *dctImage = dctTransformation(yccImage);
	writeProcessedYCCImage(dctImage, "out.txt");

	freeRGBImage(image);
	freeYCCImage(yccImage);
	freeYCCImage(dctImage);
}

void decode()
{
	ImageYCC *inputImage = readOutFileToYCCImage("out.txt");
	ImageYCC *idctImage = idctTransformation(inputImage);
	Image *outImage = yccImageToRGBImage(idctImage);
	writePPM("out.ppm", outImage);

	freeRGBImage(outImage);
	freeYCCImage(inputImage);
	freeYCCImage(idctImage);
}
