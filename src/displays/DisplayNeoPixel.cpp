#include "DisplayNeoPixel.h"

namespace udd {


    DisplayNeoPixel::DisplayNeoPixel() : Display() {}

    void DisplayNeoPixel::openDisplay(DisplayConfigruation configuration) {
        this->config = configuration;
        this->vImage = Image(config.width, config.height, BLACK);


        neopixel_init(config.stripType, config.targetFreq, config.dmaChannel, config.gpioPin, config.width * config.height);        
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

    void DisplayNeoPixel::setBrightness(int brightness) {
        config.brightness=brightness;
    }
    void DisplayNeoPixel::addGhostPixel(Point point) {
        this->ghostPixels.push_back(Point(point.x,point.y));
    }

    void DisplayNeoPixel::addGhostPixels(std::vector<Point> points) {
        for (Point p : points) {
            addGhostPixel(p);
        }
    }

    void DisplayNeoPixel::clear(Color color) {

        for (int x=0;x<config.width;++x) {
            for (int y=0;y<config.height;++y) {
                setPixel(Pixel(x,y,color));
            }
        }
        render(config.screenRotation);
    }

    void DisplayNeoPixel::showImage(Image image, Rotation rotation, ScreenMirror mirror) {

        int row=0;
        int pos=0;
        for (int y = 0; y < config.height; y++) {
            int direction=((++row)%2);
            for (int xi = 0; xi < config.width; ++xi) {
                int x=(direction)?xi:config.width-xi-1;  // data pin zig zags

                bool skip=false;
                for (Point p : ghostPixels) {
                    if (p.x==x && p.y==y) {
                        skip=true;
                        break;
                    }
                }
                if (skip) {  // ghost pixel (exists in bmp, but no physical NeoPixel available)
                    continue;
                }


                if (mirror==HORIZONTAL) {
                    x=config.width-x-1;
                } else if (mirror==VERTICAL) {
                    y=config.height-y-1;
                }
                


                int xp=x - config.xOffset;
                int yp=y - config.yOffset;
                ColorType* color = image.getPixel(xp, yp, rotation);
                    

                if (color == NULL) {
                    neopixel_setPixel(pos,0);
                } else {
                    int clr=(color->green<<16)+(color->red<<8)+(color->blue);
                    neopixel_setPixel(pos,clr);
                }
                ++pos;
            }
        }
        neopixel_setBrightness(config.brightness);
        neopixel_render();
    }


    void DisplayNeoPixel::showImage(Image image, Rotation rotation) {
        showImage(image, rotation, NORMAL);
    }


    void DisplayNeoPixel::showImage(Image image) {
        showImage(image, DEGREE_0);
    }

    void DisplayNeoPixel::render(Rotation rotation) {
        showImage(vImage, rotation);
    }
   

}
