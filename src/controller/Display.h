#pragma once
#include <inttypes.h>
#include <Color.h>

#include "Metadata.h"
#include "Image.h"


namespace udd {



    struct DisplayConfigurationStruct {
        DisplayType    displayType = ST7789;
        InterfaceType  interfacedType = SPI;
        Rotation       screenRotation = DEGREE_0;
        ScreenMirror   screenMirror = NORMAL;

        int width = 240;
        int height = 320;

        int xOffset = 0;
        int yOffset = 0;

        //                wiringPi 
        int CS = 21;
        int DC = 22;
        int RST = 23;
        int BLK = 7;


        int i2cAddress;

        int spiChannel = 0;
        int spiSpeed = 10000000;

    };

    typedef DisplayConfigurationStruct      DisplayConfigruation;

    class Display {
    protected:
        Display();
        Display(DisplayConfigruation config);

        DisplayConfigruation config;
        int handle;

    public:

        void openDisplay();

        void reset();


        void clear(Color color);

        void showImage(Image image, Rotation rotation);
        void showImage(Image image);

        virtual void init();
        virtual _word color2word(ColorType* xp);

        void printConfiguration();

        void closeSPI();
        void openSPI();
        void visable();
        void hidden();

        void writeBytes(_byte* data, uint32_t len);
        void pause();
        void resume();
        void setScreenWindow(_word Xstart, _word Ystart, _word Xend, _word Yend);
        void writeCommand(_byte data);
        void writeByte(_byte data);
        //void writeWord(_word data);
    };

}