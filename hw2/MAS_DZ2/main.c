//
//  main.c
//  MAS_DZ2
//
//  Created by Filip Gulan on 06/01/2018.
//  Copyright © 2018 Filip Gulan. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include "pgm.h"
#include "statistics.h"
#include "seq_search.h"

int main(int argc, const char * argv[])
{
    PGMImage *image1 = readPGM("lenna.pgm");
    PGMImage *image = readPGM("lenna1.pgm");
    int block = atoi("1");
    
    Point shift = findMostSimilarBlock(image1, image, block);
    printf("(%d,%d)\n", shift.x, shift.y);
    
    freePGMImage(image);
    freePGMImage(image1);
    return 0;
}
