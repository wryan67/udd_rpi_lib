#pragma once
#include "inttypes.h"
#include "Color.h"
#include "Pixel.h"

#include "Metadata.h"
#include "Image.h"
#include <mutex>

#ifndef NULL
#define NULL 0
#endif

namespace udd {
    extern std::recursive_mutex screenLock;

    struct DisplayConfigurationStruct {
        DisplayType    displayType    = UNKNOWN_DISPLAY;
        InterfaceType  interfacedType = SPI;
        Rotation       screenRotation = DEGREE_0;
        ScreenMirror   screenMirror   = NORMAL;



        int width = 240;
        int height = 320;

        int xOffset = 0;
        int yOffset = 0;

        //                wiringPi 
        int CS = 21;
        int DC = 22;
        int RST = 23;
        int BLK = -1;
        int busyPin = -1;

        int i2cAddress;

        int spiChannel = 0;
        int spiSpeed = 10000000;
        int spiMode = 0; // SPI mode 0,1,2,3


// NeoPixel
        int stripType;
        int targetFreq;
        int dmaChannel;
        int gpioPin; 
        int brightness;
    };

    typedef DisplayConfigurationStruct      DisplayConfigruation;

    Rotation validateRotation(char* rotation);

    class Display {
    protected:
        int displayId = -1;
        int handle = -1;

        Display();
        virtual ~Display() {}

        void adjustPoint(int& x, int& y, Rotation rotation);

        virtual void setWindow(Point p1, Point p2, Rotation rotation);

    public:
        DisplayConfigruation config;
        
        virtual void init();


        void openDisplay(DisplayConfigruation configuratrion);

        
        virtual void clearScreen(Color color);
        virtual void clearWindow(Color color, Point p1, Point p2, Rotation rotation);
        
        virtual void showImage(Image& image);
        virtual void showImage(Image& image, Rotation rotation);
        virtual void showImage(Image& image, Point p1, Point p2, Rotation rotation);

        virtual void readBusy() {
            fprintf(stderr, "readBusy() is not implemented for this method\n");
            exit(0);
        }

        virtual _word color2word(ColorType* xp);
        virtual void reset();


        void printConfiguration();
        void printRotation(Rotation rotation);

        void closeSPI();
        void openSPI();
        void visable();
        void hidden();

        void writeData(_byte* data, uint32_t len);
        void pause();
        void resume();
        void writeCommand(_byte data);
        void writeData(_byte data);
        //void setPixel(Pixel pixel);
        //void writeWord(_word data);
    };

}
