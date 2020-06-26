#pragma once
#include "inttypes.h"
#include "Color.h"
#include "Pixel.h"

#include "Metadata.h"
#include "Image.h"
#include <mutex>


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


// NeoPixel
        int stripType;
        int targetFreq;
        int dmaChannel;
        int gpioPin; 
        int brightness;
    };

    typedef DisplayConfigurationStruct      DisplayConfigruation;

    class Display {
    private:

    protected:

        int displayId = -1;

        Display();
        virtual ~Display() {}

        int handle = -1;

        Image vImage;

        

        
    public:
        DisplayConfigruation config;
        
        virtual void init() {
            fprintf(stderr, "init failed, base method called instead of child\n");
            exit(0);
        }

        void openDisplay(DisplayConfigruation configuratrion);

        virtual _word color2word(ColorType* xp);
        virtual void reset();
        virtual void clear(Color color);
        virtual void showImage(Image image, Rotation rotation);
        virtual void readBusy() {
            fprintf(stderr, "readBusy() is not implemented for this method\n");
            exit(0);
        }


        void printConfiguration();

        void closeSPI();
        void openSPI();
        void visable();
        void hidden();

        void showImage(Image image);
        void writeData(_byte* data, uint32_t len);
        void pause();
        void resume();
        void setScreenWindow(_word Xstart, _word Ystart, _word Xend, _word Yend);
        void writeCommand(_byte data);
        void writeData(_byte data);
        void setPixel(Pixel pixel);
        //void writeWord(_word data);
    };

}
