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

inline uint32_t   getRGB24(const ColorType *color) {
   return (color->opacity << 24 ) | (color->blue << 16) | (color->green << 8) | color->red;
}

class Color {
public:
    ColorType color;

    Color();
    Color(ColorType color);
    Color(_byte red, _byte green, _byte blue);
    Color(_byte red, _byte green, _byte blue, _byte opacity);
    Color(uint32_t colorRGB24);
    Color(const char *hexbytes);
 
    Color(int pos);

    inline ColorType toType() { return color; }
    inline uint32_t rgb24() { return getRGB24(&color); }
    inline void setColor(_byte red, _byte green, _byte blue, _byte opacity);
    inline void setRGB24(uint32_t colorRGB24);
    void      print();
    bool      equals(Color otherColor);
    bool      equals(ColorType otherColor);
    bool      equals(ColorType *otherColor);

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

