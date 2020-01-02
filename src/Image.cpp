#include "Image.h"
#include <math.h>


namespace uddImage {

    uddImage::Image::Image(int width, int height, Color backgroundColor) {
        this->width = width;
        this->height = height;

        this->backgroundColor = backgroundColor;

        uint32_t imageSize = width * height;
        uint32_t memorySize = imageSize * sizeof(ColorType);

        canvas = (ColorType*)malloc(memorySize);

        clear(backgroundColor);
    }

    void Image::clear(Color backgroundColor) {
        for (int x = 0; x < width; ++x) {
            for (int y = 0; y < height; ++y) {
                ColorType* xp = getPixel(x, y);
                xp->red = backgroundColor.color.red;
                xp->green = backgroundColor.color.green;
                xp->blue = backgroundColor.color.blue;
                xp->opacity = backgroundColor.color.opacity;
            }
        }
    }

    void Image::close() {
        free(canvas);
    }

    void Image::drawPixel(int x, int y, Color color) {
        if (x < 0) return;
        if (y < 0) return;
        if (x >= width) return;
        if (y >= height) return;

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



    void Image::printPixel(int x, int y) {
        ColorType* xp = getPixel(x, y);

        _word cx = color2word(xp);

        printf("r:%-3d g:%-3d b:%-3d %08x\n", xp->red, xp->green, xp->blue, cx);

    }



    ColorType* Image::getPixel(int x, int y) {

        if (x<0 || x > width || y<0 || y>height) {
            return NULL;
        }
        long offset = (y * width) + x;
        //    printf("getPixel:: x=%3d, y=%3d; offset=%d\n", );
        ColorType* xp = canvas + offset;

        return xp;
    }

    _word Image::color2word(ColorType* xp) {
        return ((0x1f & (xp->blue)) << 11) | ((0x3f & (xp->red)) << 5) | ((0x1f & (xp->green)));
    }

    void Image::drawPoint(int x, int y, Color color, int width) {

        switch (width) {
        case 0:
            return;
        case 1:
            drawPixel(x, y, color);
            break;
        case 2:
            drawPixel(x, y, color);
            drawPixel(x + 1, y, color);
            drawPixel(x, y + 1, color);
            drawPixel(x + 1, y + 1, color);
            break;
        case 3:
            drawPixel(x, y, color);
            drawPixel(x - 1, y, color);
            drawPixel(x + 1, y, color);

            drawPixel(x, y - 1, color);
            drawPixel(x, y + 1, color);
            break;
        default:
            fprintf(stderr, "not yet implemented (width=%d)\n", width);
            break;
        }

    }


    void Image::drawLine(int x1, int y1, int x2, int y2, Color color, LineStyle style, int width) {

        float x, y, dx, dy;
        int i, longestLeg, rx, ry;

        dx = x2 - x1;
        dy = y2 - y1;

        longestLeg = (abs(dx) >= abs(dy)) ? abs(dx) : abs(dy);

        dx = dx / longestLeg;
        dy = dy / longestLeg;
        rx = x1;
        ry = y1;
        x = x1;
        y = y1;

        int step = (style == DOTTED) ? 2 : 1;

        for (i = 0; i <= longestLeg; i += step) {

            drawPoint(rx, ry, color, width);

            if (abs(x2 - rx) == 0 && abs(y2 - ry) == 0) {
                break;
            }

            x += dx;
            y += dy;

            rx = round(x);
            ry = round(y);
        }
    }

}