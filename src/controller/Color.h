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

    inline ColorType toType() const { return color; }
    inline uint32_t rgb24() const { return getRGB24(&color); }
    inline void setColor(_byte red, _byte green, _byte blue, _byte opacity);
    inline void setRGB24(uint32_t colorRGB24);
    void      print() const;
    bool      equals(const Color &otherColor) const;
    bool      equals(const ColorType &otherColor) const;

};


// original 8
extern const Color BLACK;
extern const Color RED;
extern const Color GREEN;
extern const Color BLUE;
extern const Color YELLOW;
extern const Color MAGENTA;
extern const Color CYAN;
extern const Color WHITE;

extern const Color GRAY;
extern const Color BROWN;
extern const Color ORANGE;

extern const Color LIGHT_BLUE;
extern const Color LIGHT_GRAY;

extern const Color DARK_BLUE;
extern const Color DARK_RED;
extern const Color DARK_GREEN;

extern const Color DARK_GRAY_BLUE;

