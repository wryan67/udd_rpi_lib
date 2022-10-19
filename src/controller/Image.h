#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Color.h>
#include "inttypes.h"
#include "fonts.h"

#include "Metadata.h"


namespace udd {


    class Image {
    public:
        Image();
        Image(const Image &img);
        virtual ~Image();
        
        Image(int width, int height, Color backgroundColor);

        int getWidth();
        int getHeight();

        void clear(Color backgroundColor);

        void close();

        void drawPixel(int x, int y, Color color);

        void drawLine(int x1, int y1, int x2, int y2, Color color, LineStyle style, int width);
        void drawLineArc(int x, int y, int radius, float degree, Color color, LineStyle style, int width);

        void drawPoint(int x, int y, Color color, int width);

        void drawText(int Xstart, int Ystart, const char* pString,
            sFONT* Font, Color background, Color forground);

        void drawChar(int Xpoint, int Ypoint, const char Acsii_Char, sFONT* Font, Color Color_Background, Color Color_Foreground);

        void loadBMP(FILE* file, int Xstart, int Ystart);

        void loadBMP(const char* filename, int Xstart, int Ystart);

        void drawRectangle(int x1, int y1, int x2, int y2, Color Color, FillPattern pattern, LineStyle lineStyle, int width);

        void arcPoint(int x, int y, int length, double degree, int* xPoint, int* yPoint);

        void drawCircle(int x, int y, int radius, Color Color, FillPattern pattern, LineStyle lineStyle, int width);
        void drawPieSlice(int x, int y, int radius, float degree1, float degree2, Color color, LineStyle style, int width);
        void printPixel(int x, int y);

        ColorType* getPixel(int x, int y, udd::Rotation rotation) const;

        ColorType* getPixelColor(int x, int y) const;

        Image scale(float scaleX, float scaleY, ScaleMode mode);

    private:
        static void scaleBilinear(const Image &src, Image &dst);

        
    private:
        ColorType* canvas;

        _word width;
        _word height;
        Color    backgroundColor;

        _word color2word(ColorType* xp);
        void init();
    };
}
