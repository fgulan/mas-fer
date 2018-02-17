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

int main(int argc, const char * argv[])
{
    ImageYCC *inputImage = readOutFileToYCCImage("/Users/filipgulan/college/MAS/MAS/out.txt");
    ImageYCC *idctImage = idctTransformation(inputImage);
    Image *outImage = yccImageToRGBImage(idctImage);
    writePPM("/Users/filipgulan/college/MAS/MAS/out.ppm", outImage);
    
    freeRGBImage(outImage);
    freeYCCImage(inputImage);
    freeYCCImage(idctImage);
    return 0;
}
