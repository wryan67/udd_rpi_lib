#pragma once

#include "inttypes.h"

struct ColorStruct {
    _byte red;
    _byte green;
    _byte blue;
    _byte opacity;
    _byte filler[4];
};

typedef ColorStruct ColorType;


class Color {
public:
    ColorType color;

    Color();
    Color(ColorType color);
    Color(_byte red, _byte blue, _byte green);
    Color(const char *hexbytes);
    Color(_byte red, _byte blue, _byte green, _byte opacity);

    Color(int pos);

    ColorType toType();
    int32_t   rgb24();
    void      print();
    bool      equals(Color otherColor);
    bool      equals(ColorType otherColor);

};


// original 8
extern Color BLACK;
extern Color RED;
extern Color GREEN;
extern Color BLUE;
extern Color YELLOW;
extern Color MAGENTA;
extern Color CYAN;
extern Color WHITE;

extern Color GRAY;
extern Color BROWN;
extern Color ORANGE;

extern Color LIGHT_BLUE;
extern Color LIGHT_GRAY;

extern Color DARK_BLUE;
extern Color DARK_RED;
extern Color DARK_GREEN;

extern Color DARK_GRAY_BLUE;

