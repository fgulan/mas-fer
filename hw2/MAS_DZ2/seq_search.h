//
//  seq_search.h
//  MAS_DZ2
//
//  Created by Filip Gulan on 06/01/2018.
//  Copyright © 2018 Filip Gulan. All rights reserved.
//

#ifndef seq_search_h
#define seq_search_h

#include <stdio.h>
#include "pgm.h"

typedef struct {
    int x, y;
} Point;

typedef unsigned short pixel_t;

Point findMostSimilarBlock(PGMImage *currImage, PGMImage *previousImage, int blockIndex);
void iterate(PGMImage *currImage, int blockIndex);

#endif /* seq_search_h */
