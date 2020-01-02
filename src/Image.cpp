#include "Image.h"



Image::Image(_word width, _word height, Color backgroundColor) {
    this->width = width;
    this->height = height;

    this->backgroundColor = backgroundColor;

    uint32_t imageSize = width * height;
    uint32_t memorySize = imageSize * sizeof(ColorType);

    canvas = (ColorType *)malloc(memorySize);

    printf("Image::width=%d; height=%d; memorySize=%d\n", width, height, memorySize);

    clear(backgroundColor);
}

void Image::clear(Color backgroundColor) {
//    ColorType color = backgroundColor.toType();
    
    printf("image::clear r:%d; g:%d; b:%d sizeof(ColorType)=%d\n", backgroundColor.color.red, backgroundColor.color.green, backgroundColor.color.blue, sizeof(ColorType));

    for (int x = 0; x < width; ++x) {
        for (int y = 0; y < height; ++y) {
            ColorType* xp = getPixel(x, y);
            xp->red = backgroundColor.color.red;
            xp->green = backgroundColor.color.green;
            xp->blue = backgroundColor.color.blue;
            xp->opacity = backgroundColor.color.opacity;
        }
    }

    printf("clear::end\n"); fflush(stdout);

}

void Image::close() {
    free(canvas);
}

void Image::drawPixel(_word x, _word y, Color color) {

    ColorType* xp = getPixel(x, y);

    xp->red = color.color.red;
    xp->green = color.color.green;
    xp->blue = color.color.blue;
    xp->opacity = color.color.opacity;


    if (color.color.red != 0 ||
        color.color.green != 0 ||
        color.color.blue != 0
        ) {
    }


}

void Image::printPixel(_word x, _word y) {
    ColorType* xp = getPixel(x, y);

    _word cx = color2word(xp); 

    printf("r:%-3d g:%-3d b:%-3d %08x\n", xp->red, xp->green, xp->blue, cx);

}

ColorType *Image::getPixel(_word x, _word y) {

    if (x<0 || x > width || y<0 || y>height) {
        return NULL;
    }
    long offset = (y * width) + x;
//    printf("getPixel:: x=%3d, y=%3d; offset=%d\n", );
    ColorType *xp = canvas + offset;

    return xp;
}

_word Image::color2word(ColorType* xp) {
    return ((0x1f & (xp->blue)) << 11) | ((0x3f & (xp->red)) << 5) | ((0x1f & (xp->green)));
}
