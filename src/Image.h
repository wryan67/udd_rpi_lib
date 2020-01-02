#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Color.h>
#include <inttypes.h>


namespace uddImage {
    typedef enum {
        SOLID,
        DOTTED,
    } LineStyle;

    typedef enum {
        NONE,
        FILL,
        MASK
    } FillPattern;


    class Image {
    public:
        Image(int width, int height, Color backgroundColor);

        void clear(Color backgroundColor);

        void close();

        void drawPixel(int x, int y, Color color);

        void drawLine(int x1, int y1, int x2, int y2, Color color, LineStyle style, int width);

        void drawPoint(int x, int y, Color color, int width);

        void printPixel(int x, int y);

        ColorType* getPixel(int x, int y);

    private:
        ColorType* canvas;

        _word width;
        _word height;
        Color    backgroundColor;

        _word color2word(ColorType* xp);

    };
}
