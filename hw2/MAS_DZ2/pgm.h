//
//  pgm.h
//  MAS_DZ2
//
//  Created by Filip Gulan on 06/01/2018.
//  Copyright © 2018 Filip Gulan. All rights reserved.
//

#ifndef pgm_h
#define pgm_h

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

typedef enum { false, true } bool;

typedef struct {
    unsigned int width, height;
} Size;

typedef unsigned char PGMPixel_1;
typedef unsigned short PGMPixel_2;

typedef struct {
    Size size;
    PGMPixel_1 *data_1;
    PGMPixel_2 *data_2;
} PGMImage;

PGMImage *readPGM(const char *filename);
void freePGMImage(PGMImage *pgmImage);

#endif /* pgm_h */
