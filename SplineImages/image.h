/* Simple BMP Library */

typedef struct color {
    float r, g, b;
} Color;

typedef struct image {
    int image_width, image_height;
    Color** image_data;
} Image;

Color* ColorNew(float r, float g, float b);
Image* ImageNew(int width, int height);
void ImageSetColor(Image* image, Color* color, int x, int y);
void ImageDrawLine(Image* image, double x0, double y0, double x1, double y1);
Color* ImageGetColor(Image* image, int x, int y);
void ImageExport(Image* image, char* filename);
void ImageFreeColors(Image* image);
void ImageExampleBitmap(Image* image);