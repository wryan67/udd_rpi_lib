#include "Color.h"
#include <string.h>
#include <stdio.h>

Color::Color() {
    color.red = 0;
    color.blue = 0;
    color.green = 0;
    color.opacity = 255;
}

Color::Color(ColorType color) {
    memcpy(&this->color, &color, sizeof(ColorType));
}

Color::Color(_byte red, _byte green, _byte blue) {
    setColor(red, green, blue, 255);
}

Color::Color(_byte red, _byte green, _byte blue, _byte opacity) {
    setColor(red, green, blue, opacity);
}

Color::Color(uint32_t colorRGB24) {
    setRGB24(colorRGB24);
}
 
Color::Color(const char* hexbytes) {
    char buf[3];
    int intensity;
    int offset = 0;
    if (hexbytes[0] == '#') {
        offset = 1;
    }

    if (strlen(&hexbytes[offset]) == 5) {
        memset(buf, 0, sizeof(buf));
        strncpy(buf, &hexbytes[offset], 1);
        sscanf(buf, "%x", &intensity);
        color.red = intensity;
        offset += 1;
    } else {
        memset(buf, 0, sizeof(buf));
        strncpy(buf, &hexbytes[offset], 2);
        sscanf(buf, "%x", &intensity);
        color.red = intensity;
        offset += 2;
    }    
    
    memset(buf, 0, sizeof(buf));
    strncpy(buf, &hexbytes[offset], 2);
    sscanf(buf, "%x", &intensity);
    color.green = intensity;

    offset += 2;
    memset(buf, 0, sizeof(buf));
    strncpy(buf, &hexbytes[offset], 2);
    sscanf(buf, "%x", &intensity);
    color.blue = intensity;


    if (strlen(hexbytes) > 7) {
        offset += 2;
        memset(buf, 0, sizeof(buf));
        strncpy(buf, &hexbytes[offset], 2);
        sscanf(buf, "%x", &intensity);
        color.opacity = intensity;
    }
}

bool Color::equals(const Color &otherColor) const {
    return equals(otherColor.color);
}

bool Color::equals(const ColorType &otherColor) const {
    if (color.blue != otherColor.blue) {
        return false;
    }
    if (color.red != otherColor.red) {
        return false;
    }
    if (color.green != otherColor.green) {
        return false;
    }
    return true;
}

inline void Color::setColor(_byte red, _byte green, _byte blue, _byte opacity) {
    color.red = red;
    color.green = green;
    color.blue = blue;
    color.opacity = opacity;
}

inline void Color::setRGB24(uint32_t colorRGB24) {
    color.red = (_byte)(colorRGB24 & 0xFF);
    color.green = (_byte)((colorRGB24 & 0xFF00) >> 8);
    color.blue = (_byte)((colorRGB24 & 0xFF0000) >> 16);
    color.opacity = (_byte)((colorRGB24 & 0xFF000000) >> 24);
}

Color::Color(int clr) {
    color.opacity = 255;
    
    if (clr<0 || clr>255) {
        return;
    }

    if (clr<85) {
            color.red=clr*3;
            color.green=255-clr*3;
            color.blue=0;
    } else if (clr<170) {
            clr-= 85;
            color.red = 255 - clr * 3;
            color.green = 0;
            color.blue = clr * 3;
    } else {
            clr -= 170;
            color.red = 0;
            color.green = clr * 3;
            color.blue = 255 - clr * 3;
    }
}


void Color::print() const {

    printf("color::print: 0x%02x%02x%02x\n", this->color.red, this->color.blue,this->color.green);
    printf("color::print: 0x%02x%02x%02x\n", color.red, color.blue, color.green);

    printf("color::print: 0x%3d%3d%3d\n", this->color.red, this->color.blue,this->color.green);
    printf("color::print: 0x%3d%3d%3d\n", color.red, color.blue, color.green);

}




// Original 8
const Color BLACK             = Color(0,     0,   0);
const Color RED               = Color(255,   0,   0);
const Color GREEN             = Color(0,   255,   0);
const Color BLUE              = Color(0,     0, 255);
const Color YELLOW            = Color(255, 255,   0);
const Color MAGENTA           = Color(255,   0, 255);
const Color CYAN              = Color(0,   255, 255);
const Color WHITE             = Color(255, 255, 255);

// extended colors
const Color GRAY              = Color("#808080");
const Color BROWN             = Color(165,  42,  42);
const Color ORANGE            = Color(255, 128,   0);

const Color DARK_RED          = Color(128,   0,   0);
const Color DARK_GREEN        = Color(0,   128,   0);
const Color DARK_BLUE         = Color(0,     0, 128);


const Color LIGHT_BLUE        = Color(204, 228, 255);
const Color LIGHT_GRAY        = Color("#E0E0E0");

const Color DARK_GRAY_BLUE    = Color("003366");