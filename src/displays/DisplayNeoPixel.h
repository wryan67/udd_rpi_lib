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
        void openDisplay(DisplayConfiguration configuration);
        void printConfiguration();
        void setBrightness(int brightness);
        void render(Rotation rotation);

        void addGhostPixel(Point point);
        void addGhostPixels(std::vector<Point> points);

        void clearScreen(const Color &color);

        void showImage(const Image &image, Rotation rotation, ScreenMirror mirror);
        void showImage(const Image &image, Rotation rotation);
        void showImage(const Image &image);
        void setPixel(Pixel pixel);

    private:
        Image vImage;
        
        std::vector<Point> ghostPixels={};

    };
}
