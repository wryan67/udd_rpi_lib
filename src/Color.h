#pragma once

#include <inttypes.h>

struct ColorStruct {
    _byte red;
    _byte blue;
    _byte green;
    _byte opacity;
};

typedef ColorStruct ColorType;


class Color {
public:
    ColorType color;

    Color();
    Color(ColorType color);
    Color(_byte red, _byte blue, _byte green);
    Color(_byte red, _byte blue, _byte green, _byte opacity);
    ColorType toType();
};


extern Color BLACK;
extern Color RED;
extern Color GREEN;
extern Color BLUE;
extern Color YELOW;
extern Color MAGENTA;
extern Color WHITE;


