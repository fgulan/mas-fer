//
//  image.c
//  MAS
//
//  Created by Filip Gulan on 23/10/2017.
//  Copyright © 2017 Filip Gulan. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "image.h"

ImageYCC *createYCCImage(Size size)
{
    ImageYCC *yccImage;
    yccImage = (ImageYCC *)malloc(sizeof(ImageYCC));
    if (!yccImage) {
        fprintf(stderr, "Error: %s", "Unable to allocate memory for ImageYCC!");
        return NULL;
    }
    yccImage->size = size;
    yccImage->data = (PixelYCC *)malloc(size.width * size.height * sizeof(PixelYCC));
    if (!yccImage->data) {
        fprintf(stderr, "Error: %s", "Unable to allocate memory for ImageYCC data!");
        return NULL;
    }
    return yccImage;
}

Image *createRGBImage(Size size)
{
    Image *image;
    image = (Image *)malloc(sizeof(Image));
    if (!image) {
        fprintf(stderr, "Error: %s", "Unable to allocate memory for Image!");
        return NULL;
    }
    image->size = size;
    image->data = (Pixel *)malloc(size.width * size.height * sizeof(Pixel));
    if (!image->data) {
        fprintf(stderr, "Error: %s", "Unable to allocate memory for Image data!");
        return NULL;
    }
    return image;
}

ImageYCC *rgbImageToYCCImage(Image *image)
{
    int i;
    int size = image->size.width * image->size.height;
    Pixel rgb;
    PixelYCC yccPixel;
    ImageYCC *yccImage = createYCCImage(image->size);
    for(i = 0; i < size; i++) {
        rgb = image->data[i];
        yccPixel.y = 0.299 * rgb.red + 0.587 * rgb.green + 0.114 * rgb.blue;
        yccPixel.cb = - 0.168736 * rgb.red - 0.331264 * rgb.green + 0.5 * rgb.blue + 128;
        yccPixel.cr = 0.5 * rgb.red - 0.418688 * rgb.green - 0.081312 * rgb.blue + 128;
        yccImage->data[i] = yccPixel;
    }
    return yccImage;
}

Image *yccImageToRGBImage(ImageYCC *yccImage)
{
    int i, temp;
    int size = yccImage->size.width * yccImage->size.height;
    PixelYCC ycc;
    Pixel pixel;
    Image *image = createRGBImage(yccImage->size);
    for(i = 0; i < size; i++) {
        ycc = yccImage->data[i];
        temp = ycc.y + 1.402 * (ycc.cr - 128);
        pixel.red = MAX(0, MIN(255, temp));
        temp = ycc.y - 0.344136 * (ycc.cb - 128) - 0.714136 * (ycc.cr - 128);
        pixel.green = MAX(0, MIN(255, temp));
        temp = ycc.y + 1.772 * (ycc.cb - 128);
        pixel.blue = MAX(0, MIN(255, temp));
        image->data[i] = pixel;
    }
    return image;
}

void freeYCCImage(ImageYCC *yccImage)
{
    free(yccImage->data);
    free(yccImage);
}

void freeRGBImage(Image *image)
{
    free(image->data);
    free(image);
}
