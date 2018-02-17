//
//  statistics.c
//  MAS_DZ2
//
//  Created by Filip Gulan on 06/01/2018.
//  Copyright © 2018 Filip Gulan. All rights reserved.
//

#include "statistics.h"
#define GROUP_COUNT 16
#define PIXEL_1_MASK 0b11110000
#define PIXEL_2_MASK 0b1111000000000000

int getGroupForPixelAtIndex(PGMImage *image, size_t index)
{
    int group = -1;
    PGMPixel_1 pixel_1;
    PGMPixel_1 pixel_2;
    
    if (image->data_1) {
        pixel_1 = image->data_1[index];
        group = (pixel_1 & PIXEL_1_MASK) >> 4;
    } else if (image->data_2) {
        pixel_2 = image->data_2[index];
        group = (pixel_2 & PIXEL_2_MASK) >> 12;
    }
    if (group < 0 || group > 15) {
        group = -1;
    }
    return group;
}

void calculateAndPrintStatisticsForPGM(PGMImage *image)
{
    size_t pixel_count = image->size.width * image->size.height;
    size_t pixel_index, i;
    int groups[GROUP_COUNT] = {0};
    int group;
    
    for (pixel_index = 0; pixel_index < pixel_count; pixel_index++) {
        group = getGroupForPixelAtIndex(image, pixel_index);
        if (group < 0) {
            puts("Invalid group value!");
            return;
        }
        groups[group] += 1;
    }
    for (i = 0; i < GROUP_COUNT; i++) {
        printf("Grupa %zu -> %d\n", i, groups[i]);
    }
}
