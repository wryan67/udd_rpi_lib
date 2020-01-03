#pragma once

namespace udd {

// screen stuff
    enum DisplayType {
        ST7789, ST7735
    };

    enum InterfaceType {
        SPI, I2C
    };

    enum Rotation {
        DEGREE_0 = 0, DEGREE_90 = 90, DEGREE_180 = 180, DEGREE_270 = 270
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
