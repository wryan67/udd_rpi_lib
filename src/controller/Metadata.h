#pragma once

namespace udd {
    static const double PI = 3.14159265358979;

// screen stuff
    enum DisplayType {
        UNKNOWN_DISPLAY, ST7789, ST7735, NeoPixel
    };

    enum InterfaceType {
        SPI, I2C
    };

    enum Rotation {
        DEGREE_0 = 0, 
        DEGREE_90 = 90, 
        DEGREE_180 = 180, 
        DEGREE_270 = 270,
        DEGREE_INVALID = -1
    };

    enum ScreenMirror {
        NORMAL, HORIZONTAL, VERTICAL
    };

// drawing stuff
    typedef enum {
        SOLID,
        DOTTED,
    } LineStyle;

    typedef enum {
        NONE,
        FILL,
        MASK
    } FillPattern;



}
