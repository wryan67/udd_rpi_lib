#include "Image.h"



Image::Image(_word width, _word height, Color backgroundColor) {
    this->width = width;
    this->height = height;

    this->backgroundColor = backgroundColor;

    uint32_t imageSize = width * height;
    uint32_t memorySize = imageSize * sizeof(ColorType);

    canvas = (ColorType **)malloc(memorySize);

    printf("width=%d; height=%d; memorySize=%d\n", width, height, memorySize);

    clear(backgroundColor);
}

void Image::clear(Color backgroundColor) {
    ColorType color = backgroundColor.toType();
    for (_word x = 0; x < width; ++x) {
        for (_word y = 0; y < height; ++y) {
            _word offset = x * y * sizeof(ColorType);
            memcpy(canvas + (offset), &color, sizeof(ColorType));
        }
    }
}

void Image::close() {
    free(canvas);
}

void Image::drawPixel(_word x, _word y, Color color) {
    ColorType c1 = color.toType();

    memcpy(canvas + x * y * sizeof(ColorType), &c1, sizeof(ColorType));
}

void Image::printPixel(_word x, _word y) {
    ColorType* xp = (ColorType *) (canvas + x * y * sizeof(ColorType));
    printf("r:%d g:%d b:%d\n", xp->red, xp->green, xp->blue);

}

Color Image::getPixel(_word x, _word y) {

    if (x<0 || x > width || y<0 || y>height) {
        return BLACK;
    }

    _word offset = x * y * sizeof(ColorType);
    ColorType* xp = (ColorType*)(canvas + offset);

    return *xp;
}