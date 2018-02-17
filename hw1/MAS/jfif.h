//
//  jfif.h
//  MAS
//
//  Created by Filip Gulan on 20/10/2017.
//  Copyright © 2017 Filip Gulan. All rights reserved.
//

#ifndef jfif_h
#define jfif_h

#include "image.h"

ImageYCC *rgbImageToYCCImage(Image *image);
ImageYCC *dctTransformation(ImageYCC *image);

void writeProcessedYCCImage(ImageYCC *yccImage, char const *filename);
ImageYCC *readOutFileToYCCImage(char const *filename);
ImageYCC *idctTransformation(ImageYCC *image);

#endif /* jfif_h */
