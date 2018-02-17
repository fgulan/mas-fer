//
//  jfif.c
//  MAS
//
//  Created by Filip Gulan on 20/10/2017.
//  Copyright © 2017 Filip Gulan. All rights reserved.
//

#include "jfif.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define DCT_BLOCK 8

struct zigzag {
    int row;
    int col;
} ZigZag[8 * 8] =
{
    {0, 0},
    {0, 1}, {1, 0},
    {2, 0}, {1, 1}, {0, 2},
    {0, 3}, {1, 2}, {2, 1}, {3, 0},
    {4, 0}, {3, 1}, {2, 2}, {1, 3}, {0, 4},
    {0, 5}, {1, 4}, {2, 3}, {3, 2}, {4, 1}, {5, 0},
    {6, 0}, {5, 1}, {4, 2}, {3, 3}, {2, 4}, {1, 5}, {0, 6},
    {0, 7}, {1, 6}, {2, 5}, {3, 4}, {4, 3}, {5, 2}, {6, 1}, {7, 0},
    {7, 1}, {6, 2}, {5, 3}, {4, 4}, {3, 5}, {2, 6}, {1, 7},
    {2, 7}, {3, 6}, {4, 5}, {5, 4}, {6, 3}, {7, 2},
    {7, 3}, {6, 4}, {5, 5}, {4, 6}, {3, 7},
    {4, 7}, {5, 6}, {6, 5}, {7, 4},
    {7, 5}, {6, 6}, {5, 7},
    {6, 7}, {7, 6},
    {7, 7}
};

static const float kK1Table[8][8] =
{
    {16, 11, 10, 16, 24,  40,  51,  61},
    {12, 12, 14, 19, 26,  58,  60,  55},
    {14, 13, 16, 24, 40,  57,  69,  56},
    {14, 17, 22, 29, 51,  87,  80,  62},
    {18, 22, 37, 56, 68,  109, 103, 77},
    {24, 35, 55, 64, 81,  104, 113, 92},
    {49, 64, 78, 87, 103, 121, 120, 101},
    {72, 92, 95, 98, 112, 100, 103, 99}
};

static const float kK2Table[8][8] =
{
    {17, 18, 24, 47, 99, 99, 99, 99},
    {18, 21, 26, 66, 99, 99, 99, 99},
    {24, 26, 56, 99, 99, 99, 99, 99},
    {47, 66, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99},
    {99, 99, 99, 99, 99, 99, 99, 99}
};

ImageYCC *dctTransformation(ImageYCC *image)
{
    const float kC0 = 1.0/sqrt(2.0);
    const float kC = 1.0;
    
    ImageYCC *outImage = createYCCImage(image->size);

    int width = image->size.width;
    int xBlockCount = image->size.width / DCT_BLOCK;
    int yBlockCount = image->size.height / DCT_BLOCK;
    int blockCount = xBlockCount * yBlockCount;
    int block, yOffset, xOffset;
    PixelYCC inPixel, outPixel;
    
    float dctY[DCT_BLOCK][DCT_BLOCK]  = {0};
    float dctCb[DCT_BLOCK][DCT_BLOCK] = {0};
    float dctCr[DCT_BLOCK][DCT_BLOCK] = {0};
    
    float yVal[DCT_BLOCK][DCT_BLOCK]  = {0};
    float cbVal[DCT_BLOCK][DCT_BLOCK] = {0};
    float crVal[DCT_BLOCK][DCT_BLOCK] = {0};
    
    int kY[DCT_BLOCK][DCT_BLOCK]  = {0};
    int kCb[DCT_BLOCK][DCT_BLOCK] = {0};
    int kCr[DCT_BLOCK][DCT_BLOCK] = {0};
    
    float cu, cv, tempY, tempCb, tempCr;
    int yStart, yEnd;
    int i, j, k, l;
    
    for (block = 0; block < blockCount; block++) {
        yOffset = width * DCT_BLOCK * (block / xBlockCount);
        
        // Extract block and shift values by 128
        for (xOffset = i = 0; i < DCT_BLOCK; i++, xOffset += width) {
            yStart = (block % xBlockCount) * DCT_BLOCK + yOffset + xOffset;
            yEnd = yStart + DCT_BLOCK;
            for (j = yStart, k = 0; j < yEnd; j++, k++) {
                inPixel = image->data[j];
                yVal[i][k] = inPixel.y - 128;
                cbVal[i][k] = inPixel.cb - 128;
                crVal[i][k] = inPixel.cr - 128;
            }
        }
        
        // DCT on block
        for (i = 0; i < DCT_BLOCK; i++) {
            for (j = 0; j < DCT_BLOCK; j++) {
                cu = i == 0 ? kC0 : kC;
                cv = j == 0 ? kC0 : kC;
                tempY = tempCb = tempCr = 0;
                for (k = 0; k < DCT_BLOCK; k++) {
                    for (l = 0; l < DCT_BLOCK; l++) {
                        tempY += yVal[k][l] * cos((2 * k + 1) * i * M_PI / 16.0) * cos((2 * l + 1) * j * M_PI / 16.0);
                        tempCb += cbVal[k][l] * cos((2 * k + 1) * i * M_PI / 16.0) * cos((2 * l + 1) * j * M_PI / 16.0);
                        tempCr += crVal[k][l] * cos((2 * k + 1) * i * M_PI / 16.0) * cos((2 * l + 1) * j * M_PI / 16.0);
                    }
                }
                dctY[i][j] = 0.25 * cu * cv * tempY;
                dctCb[i][j] = 0.25 * cu * cv * tempCb;
                dctCr[i][j] = 0.25 * cu * cv * tempCr;
            }
        }
        
        // Quantization
        for (i = 0; i < DCT_BLOCK; i++) {
            for (j = 0; j < DCT_BLOCK; j++) {
                kY[i][j]  = round(dctY[i][j]  / kK1Table[i][j]);
                kCb[i][j] = round(dctCb[i][j] / kK2Table[i][j]);
                kCr[i][j] = round(dctCr[i][j] / kK2Table[i][j]);
            }
        }
        
        // Output
        for (xOffset = i = 0; i < DCT_BLOCK; i++, xOffset += width) {
            yStart = (block % xBlockCount) * DCT_BLOCK + yOffset + xOffset;
            yEnd = yStart + DCT_BLOCK;
            for (j = yStart, k = 0; j < yEnd; j++, k++) {
                outPixel.y = kY[i][k];
                outPixel.cb = kCb[i][k];
                outPixel.cr = kCr[i][k];
                outImage->data[j] = outPixel;
            }
        }
    }
    return outImage;
}

ImageYCC *idctTransformation(ImageYCC *image)
{
    const float kC0 = 1.0/sqrt(2.0);
    const float kC = 1.0;
    
    ImageYCC *outImage = createYCCImage(image->size);
    
    int width = image->size.width;
    int xBlockCount = image->size.width / DCT_BLOCK;
    int yBlockCount = image->size.height / DCT_BLOCK;
    int blockCount = xBlockCount * yBlockCount;
    int block, yOffset, xOffset;
    PixelYCC outPixel;
    
    float dctY[DCT_BLOCK][DCT_BLOCK]  = {0};
    float dctCb[DCT_BLOCK][DCT_BLOCK] = {0};
    float dctCr[DCT_BLOCK][DCT_BLOCK] = {0};
    
    float yVal[DCT_BLOCK][DCT_BLOCK]  = {0};
    float cbVal[DCT_BLOCK][DCT_BLOCK] = {0};
    float crVal[DCT_BLOCK][DCT_BLOCK] = {0};
    
    int kY[DCT_BLOCK][DCT_BLOCK]  = {0};
    int kCb[DCT_BLOCK][DCT_BLOCK] = {0};
    int kCr[DCT_BLOCK][DCT_BLOCK] = {0};
    
    float cu, cv, tempY, tempCb, tempCr;
    int yStart, yEnd;
    int i, j, k, l;
    
    for (block = 0; block < blockCount; block++) {
        yOffset = width * DCT_BLOCK * (block / xBlockCount);
        
        // Input
        for (xOffset = i = 0; i < DCT_BLOCK; i++, xOffset += width) {
            yStart = (block % xBlockCount) * DCT_BLOCK + yOffset + xOffset;
            yEnd = yStart + DCT_BLOCK;
            for (j = yStart, k = 0; j < yEnd; j++, k++) {
                kY[i][k] = image->data[j].y;
                kCb[i][k] = image->data[j].cb;
                kCr[i][k] = image->data[j].cr;
            }
        }
        
        // De-quantization
        for (i = 0; i < DCT_BLOCK; i++) {
            for (j = 0; j < DCT_BLOCK; j++) {
                dctY[i][j]  = round(kY[i][j]  * kK1Table[i][j]);
                dctCb[i][j] = round(kCb[i][j] * kK2Table[i][j]);
                dctCr[i][j] = round(kCr[i][j] * kK2Table[i][j]);
            }
        }
        
        // DCT on block
        for (i = 0; i < DCT_BLOCK; i++) {
            for (j = 0; j < DCT_BLOCK; j++) {
                tempY = tempCb = tempCr = 0;
                for (k = 0; k < DCT_BLOCK; k++) {
                    for (l = 0; l < DCT_BLOCK; l++) {
                        cu = k == 0 ? kC0 : kC;
                        cv = l == 0 ? kC0 : kC;
                        tempY += dctY[k][l] * cos((2 * i + 1) * k * M_PI / 16.0)
                                            * cos((2 * j + 1) * l * M_PI / 16.0) * cu * cv;
                        tempCb += dctCb[k][l] * cos((2 * i + 1) * k * M_PI / 16.0)
                                              * cos((2 * j + 1) * l * M_PI / 16.0) * cu * cv;
                        tempCr += dctCr[k][l] * cos((2 * i + 1) * k * M_PI / 16.0)
                                              * cos((2 * j + 1) * l * M_PI / 16.0) * cu * cv;
                    }
                }
                yVal[i][j] = 0.25 * tempY + 128 - 128;
                cbVal[i][j] = 0.25 * tempCb + 128  - 128;
                crVal[i][j] = 0.25 * tempCr + 128  - 128;
            }
        }
    
        // Output
        for (xOffset = i = 0; i < DCT_BLOCK; i++, xOffset += width) {
            yStart = (block % xBlockCount) * DCT_BLOCK + yOffset + xOffset;
            yEnd = yStart + DCT_BLOCK;
            for (j = yStart, k = 0; j < yEnd; j++, k++) {
                outPixel.y = yVal[i][k];
                outPixel.cb = cbVal[i][k];
                outPixel.cr = crVal[i][k];
                outImage->data[j] = outPixel;
            }
        }
    }
    return outImage;
}

ImageYCC *readOutFileToYCCImage(char const *filename)
{
    int block, yOffset, xOffset;
    int yStart, yEnd;
    int i, j, k, row, col, index;
    
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "Error: %s", "Unable to open input file!");
        return NULL;
    }
    Size size;
    fscanf(fp, "%d %d", &size.width, &size.height);
    while (fgetc(fp) != '\n');

    int count = size.width * size.height;
    int width = size.width;
    int blockSize = DCT_BLOCK * DCT_BLOCK;
    int xBlockCount = size.width / DCT_BLOCK;
    int yBlockCount = size.height / DCT_BLOCK;
    int blockCount = xBlockCount * yBlockCount;
    
    ImageYCC *image = createYCCImage(size);
    
    int *yValues = (int *)malloc(count * sizeof(int));
    int *cbValues = (int *)malloc(count * sizeof(int));
    int *crValues = (int *)malloc(count * sizeof(int));
    
    // Read Y values
    for (i = 0; i < count - 1; i++) {
        fscanf(fp, "%d ", &yValues[i]);
    }
    fscanf(fp, "%d", &yValues[count - 1]);
    
    // Read Cb values
    for (i = 0; i < count - 1; i++) {
        fscanf(fp, "%d ", &cbValues[i]);
    }
    fscanf(fp, "%d\n\n", &cbValues[count - 1]);
    
    // Read Cr values
    for (i = 0; i < count - 1; i++) {
        fscanf(fp, "%d ", &crValues[i]);
    }
    fscanf(fp, "%d\n", &crValues[count - 1]);
    
    int kY[DCT_BLOCK * DCT_BLOCK]  = {0};
    int kCb[DCT_BLOCK * DCT_BLOCK] = {0};
    int kCr[DCT_BLOCK * DCT_BLOCK] = {0};
    
    float yVal[DCT_BLOCK][DCT_BLOCK]  = {0};
    float cbVal[DCT_BLOCK][DCT_BLOCK] = {0};
    float crVal[DCT_BLOCK][DCT_BLOCK] = {0};
    
    PixelYCC pixel;
    
    for (block = 0; block < blockCount; block++) {
        int iStart = block * blockSize;
        int iEnd = iStart + blockSize;
        for (i = iStart; i < iEnd; i++) {
            index = i % blockSize;
            kY[index] = yValues[i];
            kCb[index] = cbValues[i];
            kCr[index] = crValues[i];
        }
        
        // Un-zigzag
        for (i = 0; i < blockSize; i++) {
            row = ZigZag[i].row;
            col = ZigZag[i].col;
            yVal[row][col] = kY[i];
            cbVal[row][col] = kCb[i];
            crVal[row][col] = kCr[i];
        }
        
        // Output
        yOffset = width * DCT_BLOCK * (block / xBlockCount);
        for (xOffset = i = 0; i < DCT_BLOCK; i++, xOffset += width) {
            yStart = (block % xBlockCount) * DCT_BLOCK + yOffset + xOffset;
            yEnd = yStart + DCT_BLOCK;
            for (j = yStart, k = 0; j < yEnd; j++, k++) {
                pixel.y = yVal[i][k];
                pixel.cb = cbVal[i][k];
                pixel.cr = crVal[i][k];
                image->data[j] = pixel;
            }
        }
    }
    
    fclose(fp);
    return image;
}

void writeZigZagRowToFile(ImageYCC *image, FILE *fp, char type)
{
    int width = image->size.width;
    int blockSize = DCT_BLOCK * DCT_BLOCK;
    int xBlockCount = image->size.width / DCT_BLOCK;
    int yBlockCount = image->size.height / DCT_BLOCK;
    int blockCount = xBlockCount * yBlockCount;
    int block, yOffset, xOffset;
    int yStart, yEnd;
    int i, j, k, row, col;
    
    int values[DCT_BLOCK][DCT_BLOCK]  = {0};
    
    for (block = 0; block < blockCount; block++) {
        yOffset = width * DCT_BLOCK * (block / xBlockCount);
        for (xOffset = i = 0; i < DCT_BLOCK; i++, xOffset += width) {
            yStart = (block % xBlockCount) * DCT_BLOCK + yOffset + xOffset;
            yEnd = yStart + DCT_BLOCK;
            for (j = yStart, k = 0; j < yEnd; j++, k++) {
                if (type == 'y') {
                    values[i][k] = (int)image->data[j].y;
                } else if (type == 'b') {
                    values[i][k] = (int)image->data[j].cb;
                } else {
                    values[i][k] = (int)image->data[j].cr;
                }
            }
        }
        bool lastBlock = block == blockCount - 1;
        
        for (i = 0; i < blockSize; i++) {
            row = ZigZag[i].row;
            col = ZigZag[i].col;
            if (lastBlock && i == blockSize - 1) {
                fprintf(fp, "%d", values[row][col]);
            } else {
                fprintf(fp, "%d ", values[row][col]);
            }
        }
    }
}

void writeProcessedYCCImage(ImageYCC *yccImage, char const *filename)
{
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: %s", "Unable to open output file!");
        return;
    }
    fprintf(fp, "%d %d\n", yccImage->size.width, yccImage->size.height);
    
    // Print Y row
    writeZigZagRowToFile(yccImage, fp, 'y');
    fprintf(fp, "\n");
    fprintf(fp, "\n");
    
    // Print Cb row
    writeZigZagRowToFile(yccImage, fp, 'b');
    fprintf(fp, "\n");
    fprintf(fp, "\n");
    
    // Print Cr row
    writeZigZagRowToFile(yccImage, fp, 'r');
    fprintf(fp, "\n");
    fclose(fp);
}
