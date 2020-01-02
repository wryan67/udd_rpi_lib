#include "Color.h"

Color::Color() {
    color.red = 0;
    color.blue = 0;
    color.green = 0;
    color.opacity = 255;
}


Color::Color(ColorType color) {
    this->color.red     = color.red;
    this->color.green   = color.green;
    this->color.blue    = color.blue;
    this->color.opacity = color.opacity;
}


Color::Color(_byte red, _byte green, _byte blue) {
    color.red       = red;
    color.green     = green;
    color.blue      = blue;
    color.opacity   = 255;
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


Color BLACK = Color(0, 0, 0);
Color RED = Color(255, 0, 0);
Color GREEN = Color(0, 255, 0);
Color BLUE = Color(0, 0, 255);
Color YELOW = Color(255, 255, 0);
Color MAGENTA = Color(255, 0, 255);
Color WHITE = Color(255, 255, 255);
