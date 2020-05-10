#include "Pixel.h"

Pixel::Pixel() {
    this->color = BLACK;
    this->point = Point(0,0);
}

Pixel::Pixel(int x, int y, Color color) {
    this->color = color;
    this->point = Point(x,y);
}

