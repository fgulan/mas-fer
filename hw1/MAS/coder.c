//
//  coder.c
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
    Image *image = readPPM(argv[1]);
    ImageYCC *yccImage = rgbImageToYCCImage(image);
    ImageYCC *dctImage = dctTransformation(yccImage);
    writeProcessedYCCImage(dctImage, "out.txt");
    
    freeRGBImage(image);
    freeYCCImage(yccImage);
    freeYCCImage(dctImage);
    return 0;
}
