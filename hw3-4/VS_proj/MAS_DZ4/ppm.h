//
//  ppm.h
//  MAS
//
//  Created by Filip Gulan on 20/10/2017.
//  Copyright © 2017 Filip Gulan. All rights reserved.
//

#ifndef ppm_h
#define ppm_h

#include <stdio.h>
#include "image.h"

Image *readPPM(const char *filename);
bool writePPM(const char *filename, Image *image);

#endif /* ppm_h */
