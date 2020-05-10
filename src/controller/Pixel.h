#pragma once


#include "Color.h"
#include "Point.h"

class Pixel {
public:
    Color color;
    Point point;

    Pixel();
    Pixel(int x, int y, Color color);
    Pixel(int x, int y, ColorType color);
};


