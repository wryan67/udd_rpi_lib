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
    this->color.red = color.red;
    this->color.green = color.green;
    this->color.blue = color.blue;
    this->color.opacity = color.opacity;
}


Color::Color(_byte red, _byte green, _byte blue) {
    color.red = red;
    color.green = green;
    color.blue = blue;
    color.opacity = 255;
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

bool Color::equals(Color otherColor) {
    if (this->color.blue != otherColor.color.blue) {
        return false;
    }
    if (this->color.red!= otherColor.color.red) {
        return false;
    }
    if (this->color.green != otherColor.color.green) {
        return false;
    }

}

Color::Color(_byte red, _byte green, _byte blue, _byte opacity) {
    color.red       = red;
    color.green     = green;
    color.blue      = blue;
    color.opacity   = opacity;
}

ColorType Color::toType() {
    return color;
}


int32_t Color::rgb24() {
    return (color.green<<16)+(color.red<<8)+(color.blue);
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


void Color::print() {

    printf("color::print: 0x%02x%02x%02x\n", this->color.red, this->color.blue,this->color.green);
    printf("color::print: 0x%02x%02x%02x\n", color.red, color.blue, color.green);

    printf("color::print: 0x%3d%3d%3d\n", this->color.red, this->color.blue,this->color.green);
    printf("color::print: 0x%3d%3d%3d\n", color.red, color.blue, color.green);

}




// Original 8
Color BLACK             = Color(0,     0,   0);
Color RED               = Color(255,   0,   0);
Color GREEN             = Color(0,   255,   0);
Color BLUE              = Color(0,     0, 255);
Color YELLOW            = Color(255, 255,   0);
Color MAGENTA           = Color(255,   0, 255);
Color CYAN              = Color(0,   255, 255);
Color WHITE             = Color(255, 255, 255);

// extended colors
Color GRAY              = Color("#808080");
Color BROWN             = Color(165,  42,  42);
Color ORANGE            = Color(255, 128,   0);

Color DARK_RED          = Color(128,   0,   0);
Color DARK_GREEN        = Color(0,   128,   0);
Color DARK_BLUE         = Color(0,     0, 128);


Color LIGHT_BLUE        = Color(204, 228, 255);
Color LIGHT_GRAY        = Color("#E0E0E0");

Color DARK_GRAY_BLUE    = Color("003366");