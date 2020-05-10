#include <DisplayNeoPixel.h>

namespace udd {
    DisplayNeoPixel::DisplayNeoPixel() : Display() {}

    void DisplayNeoPixel::openDisplay(DisplayConfigruation configuration) {
        this->config = configuration;

        neopixel_init(config.stripType, config.targetFreq, config.dmaChannel, config.gpioPin, config.width * config.height);        
        neopixel_setBrighness(config.brightness);
    }

    void DisplayNeoPixel::printConfiguration() {
        printf("displayId:         %d\n", displayId);
        printf("width:             %d\n", config.width);
        printf("height:            %d\n", config.height);

        printf("stripType:         %d\n", config.stripType);
        printf("targetFreq:        %d\n", config.targetFreq);
        printf("dmaChannel:        %d\n", config.dmaChannel);
        printf("gpioPin:           %d\n", config.gpioPin);


        BLUE.print();

        printf("blue=%d %d %d\n",  BLUE.color.blue, BLUE.color.red, BLUE.color.green);

    }

    void DisplayNeoPixel::addGhostPixel(Point point) {
        this->ghostPixels.push_back(Point(point.x,point.y));
    }
    void DisplayNeoPixel::addGhostPixels(std::vector<Point> points) {
        for (Point p : points) {
            addGhostPixel(p);
        }
    }

    void DisplayNeoPixel::setPixel(Pixel pixel) {
        int color=pixel.color.rgb24();
        // printf("neopixel::setPixel x=%3d y=%3d, color=0x%6x\n",pixel.point);
        neopixel_setPixel(pixel.point.x*pixel.point.y,color);
    }


    void DisplayNeoPixel::clear(Color color) {

        printf("neopixel::witch=%d height=%d\n", config.width, config.height);

        for (int x=0;x<config.width;++x) {
            for (int y=0;y<config.height;++y) {
                setPixel(Pixel(x,y,color));
            }
        }
        neopixel_render();
    }
}