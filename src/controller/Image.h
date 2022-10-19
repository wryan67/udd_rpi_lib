#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Color.h"
#include "inttypes.h"
#include "fonts.h"

#include "Metadata.h"


namespace udd {


    class Image {
    public:
        Image();
        Image(const Image &img);
        Image(Image &&img);
        Image& operator=(const Image &img);
        Image& operator=(Image &&img);
        virtual ~Image();
        
        Image(int width, int height, const Color &backgroundColor);

        int getWidth() const;
        int getHeight() const;

        void clear(const Color &backgroundColor);

        void close();

        void drawPixel(int x, int y, const Color &color);
        void drawPixel(int x, int y, const ColorType &color);

        void drawLine(int x1, int y1, int x2, int y2, const Color &color, LineStyle style, int width);
        void drawLineArc(int x, int y, int radius, float degree, const Color &color, LineStyle style, int width);

        void drawPoint(int x, int y, const Color &color, int width);

        void drawText(int Xstart, int Ystart, const char* pString,
            sFONT* Font, const Color &background, const Color &forground);

        void drawChar(int Xpoint, int Ypoint, const char Acsii_Char, sFONT* Font, const Color &Color_Background, const Color &Color_Foreground);

        void loadBMP(FILE* file, int Xstart, int Ystart);

        void loadBMP(const char* filename, int Xstart, int Ystart);

        void drawRectangle(int x1, int y1, int x2, int y2, const Color &Color, FillPattern pattern, LineStyle lineStyle, int width);

        void arcPoint(int x, int y, int length, double degree, int* xPoint, int* yPoint);

        void drawCircle(int x, int y, int radius, const Color &Color, FillPattern pattern, LineStyle lineStyle, int width);
        void drawPieSlice(int x, int y, int radius, float degree1, float degree2, const Color &color, LineStyle style, int width);
        void printPixel(int x, int y);

        ColorType* getPixel(int x, int y, udd::Rotation rotation) const;

        ColorType* getPixelColor(int x, int y) const;

        Image scale(float scaleX, float scaleY, ScaleMode mode);
        Image rotate(float angle, AngleUnit units);

    private:
        static void scaleBilinear(const Image &src, Image &dst);
        Image simpleRotate(udd::Rotation rotation);
        Image threeShearRotate(float angleRads);

        
    private:
        ColorType* canvas;

        _word width;
        _word height;
        Color    backgroundColor;

        _word color2word(ColorType* xp);
        void init();
        void copy(const Image& img);
    };
}
