#include "DisplayNeoPixel.h"

namespace udd {


    DisplayNeoPixel::DisplayNeoPixel() : Display() {}

    void DisplayNeoPixel::openDisplay(DisplayConfigruation configuration) {
        this->config = configuration;
        this->vImage = Image(config.width, config.height, BLACK);


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

    void DisplayNeoPixel::clear(Color color) {

        for (int x=0;x<config.width;++x) {
            for (int y=0;y<config.height;++y) {
                setPixel(Pixel(x,y,color));
            }
        }
        render(config.screenRotation);
    }

    void DisplayNeoPixel::render(Rotation rotation) {


        
    //    int clr=(color->green<<16)+(color->red<<8)+(color->blue);

      //  int color=pixel.color.rgb24();

        printf("rendering...\n");

        int pos=0;
        for (int y = 0; y < config.height; y++) {
            for (int x = 0; x < config.width; ++x) {
                bool skip=false;
                for (Point p : ghostPixels) {
                    if (p.x==x && p.y==y) {
                        skip=true;
                        break;
                    }
                }
                if (skip) {
                    printf(" [    ]");
                    continue;
                }

                ColorType* color = vImage.getPixel(x - config.xOffset, y - config.yOffset, rotation);
                    

                if (color == NULL) {
                    printf(" ------");
                    neopixel_setPixel(pos,0);
                } else {
                    int clr=(color->green<<16)+(color->red<<8)+(color->blue);
                    printf(" %02x%02x%02x",color->red, color->blue, color->green);
                    neopixel_setPixel(pos,clr);
                }
                ++pos;
            }
            printf("\n");

//            writeBytes(rowPointer, (config.width + config.xOffset) * 2);
        }



        neopixel_render();
    }


    void DisplayNeoPixel::showImage(Image image) {
        showImage(image, DEGREE_0);
    }

    void DisplayNeoPixel::showImage(Image image, Rotation rotation) {

        int width = config.width + config.xOffset;
        int height = config.height + config.yOffset;

        printf("width=%d height=%d xOffset=%d yOffset=%d\n",config.width, config.height, config.xOffset,config.yOffset);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; ++x) {
                ColorType* ct = image.getPixel(x - config.xOffset, y - config.yOffset, rotation);
                vImage.drawPoint(x,y,*ct,1);
            }
        }


        printf("\n");
        render(config.screenRotation);
    }

}