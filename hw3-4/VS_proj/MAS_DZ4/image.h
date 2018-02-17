//
//  image.h
//  MAS
//
//  Created by Filip Gulan on 20/10/2017.
//  Copyright © 2017 Filip Gulan. All rights reserved.
//

#ifndef image_h
#define image_h

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#pragma warning(disable:4996)

typedef enum { false, true } bool;

typedef struct {
    unsigned int width, height;
} Size;

typedef struct {
    float y, cb, cr;
} PixelYCC;

typedef struct {
    Size size;
    PixelYCC *data;
} ImageYCC;

typedef struct {
    unsigned char red, green, blue;
} Pixel;

typedef struct {
    Size size;
    Pixel *data;
} Image;

Image *createRGBImage(Size size);
ImageYCC *createYCCImage(Size size);

ImageYCC *rgbImageToYCCImage(Image *image);
Image *yccImageToRGBImage(ImageYCC *image);

void freeYCCImage(ImageYCC *yccImage);
void freeRGBImage(Image *image);

#endif /* image_h */
