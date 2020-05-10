#pragma once

#define WS2811_TARGET_FREQ                       800000   // Can go as low as 400000

#define WS2812_STRIP                             WS2811_STRIP_GRB
#define SK6812_STRIP                             WS2811_STRIP_GRB
#define SK6812W_STRIP                            SK6812_STRIP_GRBW


#include "Display.h"
#include "Pixel.h"

#include <neopixel.h>

#include <vector>




namespace udd {

    class DisplayNeoPixel : public Display {
        
    public:

        DisplayNeoPixel();
        void openDisplay(DisplayConfigruation configuration);
        void printConfiguration();

        void addGhostPixel(Point point);
        void addGhostPixels(std::vector<Point> points);

        void clear(Color color);

        void setPixel(Pixel pixel);

        // void showImage(Image image, Rotation rotation);
        // void showImage(Image image);


    private:
        std::vector<Point> ghostPixels={};

    };
}
