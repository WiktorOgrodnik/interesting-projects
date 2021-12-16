#include "image.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

Color* ColorNew(float r, float g, float b) {
    Color* t = NULL;
    if ((t = (Color*)malloc(sizeof(Color))) == 0) {
        printf("ColorNew - Erorr1: Can not allocate Color!\n");
        exit(EXIT_FAILURE);
    }

    t->b = b;
    t->g = g;
    t->r = r;

    return t;
}

Image* ImageNew(int width, int height) {
    Image* t;
    if ((t = (Image*)malloc(sizeof(Image))) == 0) {
        printf("ImageNew - Erorr1: Can not allocate Image!\n");
        exit(EXIT_FAILURE);
    }
    
    t->image_width = width;
    t->image_height = height;

    t->image_data = (Color**)malloc(sizeof(Color*)*width*height);

    int iter = width*height;
    for (int i = 0; i < iter; i++) {
        t->image_data[i] = ColorNew(1.f, 1.f, 1.f);
    }

    return t;
}

void ImageSetColor(Image* image, Color* color, int x, int y) {
    if (!image || !color) {
        printf("ImageSetColor - Erorr1: There is no Image or Color\n");
        exit(EXIT_FAILURE);
    }

    if (x >= image->image_width || y >= image->image_height) return;
    
    if (image->image_data[y * image->image_width + x])
        free(image->image_data[y * image->image_width + x]);
    
    image->image_data[y * image->image_width + x] = color;
}

static void rasLineLow(Image* image, double x0, double y0, double x1, double y1) {
    double dx = x1 - x0;
    double dy = y1 - y0;
    double yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }
    double p = 2*dy - dx;
    double y = y0;

    for (int x = x0; x <= x1; x++) {
        ImageSetColor(image, ColorNew(0, 0, 0), x, y);
        if (p > 0) {
            y += yi;
            p += 2 * (dy - dx);
        } else p += 2*dy;
    }
}

static void rasLineHigh(Image* image, double x0, double y0, double x1, double y1) {
    double dx = x1 - x0;
    double dy = y1 - y0;
    double xi = 1;
    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }
    double p = 2*dx - dy;
    double x = x0;

    for (int y = y0; y <= y1; y++) {
        ImageSetColor(image, ColorNew(0, 0, 0), x, y);
        if (p > 0) {
            x += xi;
            p += 2 * (dx - dy);
        } else p += 2*dx;
    }
}

void ImageDrawLine(Image* image, double x0, double y0, double x1, double y1) {

    if (fabs(y1 - y0) < fabs(x1 - x0)) {
        if (x0 > x1) rasLineLow(image, x1, y1, x0, y0);
        else rasLineLow(image, x0, y0, x1, y1);
    } else {
        if (y0 > y1) rasLineHigh(image, x1, y1, x0, y0);
        else rasLineHigh(image, x0, y0, x1, y1);
    }
}

Color* ImageGetColor(Image* image, int x, int y) {
    if (!image) {
        printf("ImageGetColor - Erorr1: There is no Image\n");
        exit(EXIT_FAILURE);
    }

    return image->image_data[y * image->image_width + x];
}

void ImageExport(Image* image, char* filename) {
    if (!image) {
        printf("ImageExport - Erorr1: There is no Image\n");
        exit(EXIT_FAILURE);
    }

    FILE* f = fopen(filename, "wb");

    if (!f) {
        printf("ImageExport - Erorr2: Could not create a bitmap file\n");
        exit(EXIT_FAILURE);
    }
    
    char bmpPadding[3] = {0, 0, 0};
    int paddingAmmount = (4 - (image->image_width * 3) % 4) % 4;

    const int fileHeaderSize = 14;
    const int fileInformationSize = 40;
    const int fileDataSize = image->image_width * image->image_height * 3 + paddingAmmount * image->image_height;

    const int fileSize = fileHeaderSize + fileInformationSize + fileDataSize;

    char fileHeader[fileHeaderSize];

    //File type
    fileHeader[0] = 'B';
    fileHeader[1] = 'M';
    //File size
    fileHeader[2] = fileSize;
    fileHeader[3] = fileSize >> 8;
    fileHeader[4] = fileSize >> 16;
    fileHeader[5] = fileSize >> 24;
    //Reserved Space
    fileHeader[6] = 0;
    fileHeader[7] = 0;
    fileHeader[8] = 0;
    fileHeader[9] = 0;
    //Pixel Data Offset
    fileHeader[10] = fileHeaderSize + fileInformationSize;
    fileHeader[11] = 0;
    fileHeader[12] = 0;
    fileHeader[13] = 0;

    char fileInformation[fileInformationSize];

    //Information Size
    fileInformation[0] = fileInformationSize;
    fileInformation[1] = 0;
    fileInformation[2] = 0;
    fileInformation[3] = 0;
    //Image width
    fileInformation[4] = image->image_width;
    fileInformation[5] = image->image_width >> 8;
    fileInformation[6] = image->image_width >> 16;
    fileInformation[7] = image->image_width >> 24;
    //Image hidth
    fileInformation[8] = image->image_height;
    fileInformation[9] = image->image_height >> 8;
    fileInformation[10] = image->image_height >> 16;
    fileInformation[11] = image->image_height >> 24;
    //Planes
    fileInformation[12] = 1;
    fileInformation[13] = 0;
    //Bits per pixel (RGB)
    fileInformation[14] = 24;
    //Other unused
    for (int i = 15; i <= 39; i++) fileInformation[i] = 0;

    fwrite(fileHeader, sizeof(char), 14, f);
    fwrite(fileInformation, sizeof(char), 40, f);

    for (int y = 0; y < image->image_height; y++) {
        for (int x = 0; x < image->image_width; x++) {
            char r = (char)(ImageGetColor(image, x, y)->r * 255.f);
            char g = (char)(ImageGetColor(image, x, y)->g * 255.f);
            char b = (char)(ImageGetColor(image, x, y)->b * 255.f);

            char rgb[3] = {r, g, b};
            fwrite(rgb, sizeof(char), 3, f);
        }
        fwrite(bmpPadding, sizeof(char), paddingAmmount, f);
    }

    fclose(f);
}

void ImageFreeColors(Image* image) {
    if (!image) {
        printf("ImageFreeColors - Erorr1: There is no Image\n");
        exit(EXIT_FAILURE);
    }

    int iter = image->image_height * image->image_width;
    for (int i = 0; i < iter; i++) {
        free(image->image_data[i]);
    }
}

void ImageExampleBitmap(Image* image) {

    if (!image) {
        printf("ImageExampleBitmap - Erorr1: There is no Image\n");
        exit(EXIT_FAILURE);
    }

    for (int y = 0; y < image->image_height; y++) {
        for (int x = 0; x < image->image_width; x++) {
            ImageSetColor(image, ColorNew((float)x / (float)image->image_width, 1.f - ((float)x / (float)image->image_width), (float)y / (float)image->image_height), x, y);
        }
    }
}