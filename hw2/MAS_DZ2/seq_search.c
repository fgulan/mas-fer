//
//  seq_search.c
//  MAS_DZ2
//
//  Created by Filip Gulan on 06/01/2018.
//  Copyright © 2018 Filip Gulan. All rights reserved.
//

#include "seq_search.h"
#include <math.h>
#include <float.h>
#include <stdlib.h>

#define BLOCK_SIZE 16
#define WINDOW_SIZE 16

void printError(const char *message)
{
    fprintf(stderr, "Error: %s\n", message);
}

bool sameSize(Size size1, Size size2)
{
    return size1.height == size2.height && size1.width == size2.width;
}

pixel_t getPixel(PGMImage *image, int index)
{
    if (image->data_1) {
        return image->data_1[index];
    } else {
        return image->data_2[index];
    }
}

int arrayIndexFromPoints(int x, int y, Size size)
{
    return y * size.width + x;
}

int arrayIndexFromBlockIndex(int blockIndex, int xBlockCount, Size size)
{
    int x = (blockIndex % xBlockCount) * BLOCK_SIZE;
    int y = (blockIndex / xBlockCount) * BLOCK_SIZE;
    return arrayIndexFromPoints(x, y, size);
}

Point positionFromArrayIndex(int arrayIndex, Size size)
{
    Point position;
    position.x = arrayIndex % size.width;
    position.y = arrayIndex / size.height;
    return position;
}

Point positionFromBlockIndex(int blockIndex, int xBlockCount, Size size)
{
    int arrayIndex = arrayIndexFromBlockIndex(blockIndex, xBlockCount, size);
    return positionFromArrayIndex(arrayIndex, size);
}

void fillBlock(pixel_t block[][BLOCK_SIZE], pixel_t value)
{
    int i, j;
    for (i = 0; i < BLOCK_SIZE; i++) {
        for (j = 0; j < BLOCK_SIZE; j++) {
            block[i][j] = value;
        }
    }
}

void extractBlock(PGMImage *image, Point position, pixel_t block[][BLOCK_SIZE])
{
    int i, j, x, y, index;
    Size size = image->size;
    fillBlock(block, 0);
    
    for (i = 0, y = position.y; i < BLOCK_SIZE && y < size.height; i++, y++) {
        for (j = 0, x = position.x; j < BLOCK_SIZE && x < size.width; j++, x++) {
            index = arrayIndexFromPoints(x, y, size);
            block[i][j] = getPixel(image, index);
        }
    }
}

int pad(int value, int padding)
{
    int distance = value % padding;
    if (distance > 0) {
        value += padding - distance;
    }
    return value;
}

float MAD(pixel_t block1[][BLOCK_SIZE], pixel_t block2[][BLOCK_SIZE])
{
    int i, j;
    float mad = 0.f;
    for (i = 0; i < BLOCK_SIZE; i++) {
        for (j = 0; j < BLOCK_SIZE; j++) {
            mad += abs(block1[i][j] - block2[i][j]);
        }
    }
    return mad/(float)(BLOCK_SIZE * BLOCK_SIZE);
}

Point findMostSimilarBlock(PGMImage *currImage, PGMImage *prevImage, int blockIndex)
{
    Point shift = {0, 0}, temp;
    Size size = currImage->size;
    
    // Size mismatch
    if (!sameSize(currImage->size, prevImage->size)) {
        return shift;
    }

    int xBlockCount = ceil((float)size.width / BLOCK_SIZE);
    int yBlockCount = ceil((float)size.height / BLOCK_SIZE);
    int blockCount = xBlockCount * yBlockCount;
    
    // Invalid block index check
    if (blockIndex < 0 || blockIndex >= blockCount) {
        return shift;
    }
    
    pixel_t targetBlock[BLOCK_SIZE][BLOCK_SIZE] = {{0}};
    pixel_t block[BLOCK_SIZE][BLOCK_SIZE] = {{0}};
    
    
    int paddedWidth = pad(size.width, BLOCK_SIZE), paddedHeight = pad(size.height, BLOCK_SIZE);
    Point targetPosition = positionFromBlockIndex(blockIndex, xBlockCount, size);
    int startX = MAX(targetPosition.x - WINDOW_SIZE, 0),
        startY = MAX(targetPosition.y - WINDOW_SIZE, 0),
        endX = MIN(targetPosition.x + WINDOW_SIZE, paddedWidth - WINDOW_SIZE),
        endY = MIN(targetPosition.y + WINDOW_SIZE, paddedHeight - WINDOW_SIZE);
    int i, j;
    float currentMAD, minMAD = FLT_MAX;
    // Extract block from target position
    extractBlock(currImage, targetPosition, targetBlock);
    
    for (i = startY; i <= endY; i++) {
        temp.y = i;
        for (j = startX; j <= endX; j++) {
            temp.x = j;
            extractBlock(prevImage, temp, block);
            currentMAD = MAD(targetBlock, block);
            if (currentMAD < minMAD) {
                minMAD = currentMAD;
                shift.x = -(targetPosition.x - temp.x);
                shift.y = -(targetPosition.y - temp.y);
            }
        }
    }
    
    return shift;
}
