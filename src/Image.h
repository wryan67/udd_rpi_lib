#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Display.h>
#include <Color.h>
#include <inttypes.h>


class Image {
public:


    Image(_word width, _word height, Color backgroundColor);

    void clear(Color backgroundColor);

    void close();
    
    void drawPixel(_word x, _word y, Color color);

    void printPixel(_word x, _word y);

    ColorType *getPixel(_word x, _word y);

    _word color2word(ColorType* xp);


private:
    ColorType *canvas;

    _word width;
    _word height;
    Color    backgroundColor;

};

