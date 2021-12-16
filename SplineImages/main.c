#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "image.h"

#define LAMBDA 0.5
#define e16 1.0/6.0

typedef struct point {
    double x, y;
} Point;

double hk(int k) {
    return 1.0/(double)k;
}

void fillqp(double q[], double p[], double t[], int size) {
    q[0] = 0;

    for (int i = 1; i < size - 1; i++) {
        p[i] = (LAMBDA * q[i-1]) + 2.0;
        q[i] = (LAMBDA - 1.0)/p[i];
    }
}

void filld(double b[], double f[], int size) {

    double mv = 0.0;

    for (int i = 0; i < 3; i++) {
        for (int j = i; j < size; j++) {
            if (i == 0) b[j] = f[j];
            else {
                double mv2 = b[j];
                b[j] = (b[j] - mv)/(i*hk(size - 1));
                mv = mv2;
            }
        }

        mv = b[i];
    }

    for (int i = 0; i < size - 1; i++) {
        b[i] *= 6.0;
    }
}

void fillum(double u[], double m[], double d[], double p[], double q[], double t[], int size) {
    u[0] = 0;
    for (int i = 1; i < size - 1; i++) {
        u[i] = (d[i+1] - LAMBDA * u[i-1])/p[i];
    }

    m[size - 2] = u[size - 2];
    for (int i = size - 3; i > 0; i--) {
        m[i] = u[i] + q[i]*m[i+1];
    }
}

double exef(double x, int k, double m[], double xk[], double f[], int size) {
    double h = hk(size - 1);
    return (e16*m[k-1]*pow(xk[k] - x, 3) + e16*m[k]*pow(x - xk[k-1], 3) + (f[k-1] - e16*m[k-1]*h*h)*(xk[k] - x) + (f[k] - e16*m[k]*h*h)*(x - xk[k-1]))/h;
}

int findIndex(double x, int size) {
    return (int)(x*(size - 1)) + 1 == size ? (size - 1) : (int)(x*(size - 1)) + 1;
}

int main(int argc, char** argv) {

    char* xFileName = "xdata.txt";
    char* yFileName = "ydata.txt";
    FILE* xdata, *ydata;

    int M = 1000;
    if (argc == 2) M = atoi(argv[1]);
    if (argc == 4) {
        M = atoi(argv[1]);
        xFileName = argv[2];
        yFileName = argv[3];
    }

    if ((xdata = fopen(xFileName, "r")) == NULL) {
        printf ("Can not open file: %s\n", xFileName);
        exit(EXIT_FAILURE);
    }
    if ((ydata = fopen(yFileName, "r")) == NULL) {
        printf ("Can not open file: %s\n", yFileName);
        exit(EXIT_FAILURE);
    }
    
    int xsize = 0, ysize = 0;
    double x[1000];
    double y[1000];

    while (fscanf(xdata, "%lf", &x[xsize++]) != EOF);
    while (fscanf(ydata, "%lf", &y[ysize++]) != EOF);

    if (--xsize != --ysize) {
        printf ("Number of arguments in x and y files is not accurate!\n");
        exit(EXIT_FAILURE);
    }

    double t[xsize];

    for (int i = 0; i < xsize; i++) t[i] = (double)i / (double)(xsize - 1);

    fclose(xdata);
    fclose(ydata);

    double q[xsize];
    double p[xsize];
    double u[xsize];
    double d[xsize];
    double mx[xsize];
    double my[xsize];

    fillqp(q, p, t, xsize);

    // SFunctionsX

    filld(d, x, xsize);
    fillum(u, mx, d, p, q, t, xsize);
    
    // SFunctionsY

    filld(d, y, xsize);
    fillum(u, my, d, p, q, t, xsize);

    Point* points[M + 1];
    double maxPointX = 0, maxPointY = 0;
    double minPointX = 0, minPointY = 0;

    for (int i = 0; i <= M; i++) {
        double ut = (double)i/(double)M;

        points[i] = (Point*)malloc(sizeof(Point));
        points[i]->x = exef(ut, findIndex(ut, xsize), mx, t, x, xsize);
        points[i]->y = exef(ut, findIndex(ut, xsize), my, t, y, xsize);

        if (i == 0) {
            maxPointX = minPointX = points[i]->x;
            maxPointY = minPointY = points[i]->y;
        } else {
            if (points[i]->x > maxPointX) maxPointX = points[i]->x;
            if (points[i]->x < minPointX) minPointX = points[i]->x;
            if (points[i]->y > maxPointY) maxPointY = points[i]->y;
            if (points[i]->y < minPointY) minPointY = points[i]->y;
        }
    }
    
    // Here you can change the image resolution
    int width = 1280;
    int height = 720;
    Image* image = ImageNew(width, height);

    for (int i = 0; i <= M; i++) {
        points[i]->x = (points[i]->x - minPointX) * (width / (maxPointX - minPointX));
        points[i]->y = (points[i]->y - minPointY) * (height / (maxPointY - minPointY));

        if (i > 0) ImageDrawLine(image, points[i-1]->x, points[i-1]->y, points[i]->x, points[i]->y);
    }
    
    ImageExport(image, "image.bmp");

    ImageFreeColors(image);
    free(image);

    return 0;
}