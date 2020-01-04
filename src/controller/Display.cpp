#include "Display.h"
#include "Image.h"

#include <unistd.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <pthread.h>

namespace udd {
    bool initDisplay = false;
    pthread_mutex_t screenLock;

    Display::Display() {
        if (!initDisplay) {
            initDisplay = true;
            if (pthread_mutex_init(&screenLock, NULL) != 0) {
                printf("\nDisplay::screen lock mutex init has failed\n");
                exit(2);
            }
            printf("mutex initialized\n"); fflush(stdout);
        }
    }


    void Display::openDisplay(DisplayConfigruation configuration) {
        this->config = configuration;

        openSPI();

        pinMode(config.CS,  OUTPUT);
        pinMode(config.RST, OUTPUT);
        pinMode(config.DC,  OUTPUT);
        pinMode(config.BLK, OUTPUT);

        visable();

        this->init();
    }

    void Display::reset() {
        printf("CS=%d  DC=%d  RST=%d\n", config.CS, config.DC, config.RST);
        digitalWrite(config.DC, 1);
        digitalWrite(config.CS, 1);
        digitalWrite(config.RST, 1);
        usleep(100 * 1000);
        digitalWrite(config.RST, 0);
        usleep(100 * 1000);
        digitalWrite(config.RST, 1);
        usleep(100 * 1000);
    }

    _word Display::color2word(ColorType* xp) {
        double bluePct = xp->blue / 255.0;
        double greenPct = xp->green / 255.0;
        double redPct = xp->red / 255.0;

        int red = redPct * 0x1f;
        int green = greenPct * 0x3f;
        int blue = bluePct * 0x1f;

        _word buf = ((0x1f & (red)) << 11) | ((0x3f & (green)) << 5) | ((0x1f & (blue)));

        return (buf >> 8) | buf << 8;
    }



    void Display::printConfiguration() {
        printf("width:             %d\n", config.width);
        printf("height:            %d\n", config.height);
        printf("xOffset:           %d\n", config.xOffset);
        printf("yOffset:           %d\n", config.yOffset);
        printf("cs:                %d\n", config.CS);
        printf("dc:                %d\n", config.DC);
        printf("rst:               %d\n", config.RST);
        printf("blk:               %d\n", config.BLK);

        printf("spiChannel:        %d\n", config.spiChannel);
        printf("spiSpeed:          %d\n", config.spiSpeed);
        printf("handle:            %d\n", handle);

    }



    void Display::closeSPI() {
        close(handle);
    }

    void Display::openSPI() {
        if (handle >= 0) {
            close(handle);
        }
        handle = wiringPiSPISetup(config.spiChannel, config.spiSpeed);
    }

    void Display::visable() {
        digitalWrite(config.BLK, 1);
    }

    void Display::hidden() {
        digitalWrite(config.BLK, 0);
    }

    void Display::clear(Color color) {
        pthread_mutex_lock(&screenLock);
        openSPI();
        resume();

        ColorType ct = color.toType();

        _word  row[config.width+config.xOffset];
        _byte* rowPointer = (_byte*)(row);
        _word  cx = color2word(&ct);

        for (int x = 0; x < config.width+config.xOffset; x++) {
            row[x] = cx;
        }

        setScreenWindow(0, 0, config.width+config.xOffset, config.height+config.yOffset);
        digitalWrite(config.DC, 1);

        for (int y = 0; y < config.height+config.yOffset; y++) {
            writeBytes(rowPointer, (config.width+config.xOffset) * 2);
        }
        pause();
        pthread_mutex_unlock(&screenLock);

    }
    

    void Display::showImage(Image image, Rotation rotation) {
        pthread_mutex_lock(&screenLock);
        openSPI();

        int width  = config.width  + config.xOffset;
        int height = config.height + config.yOffset;

        setScreenWindow(0, 0, width, height);
        digitalWrite(config.DC, 1);
        digitalWrite(config.CS, 0);

        _word  row[config.width];
        _byte* rowPointer = (_byte*)(row);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; ++x) {
                ColorType* ct= image.getPixel(x-config.xOffset,y-config.yOffset,rotation);

                if (ct == NULL) {
                    row[x] = 0;
                } else {
                    row[x] = color2word(ct);
                }
            }

            writeBytes(rowPointer, (config.width + config.xOffset) * 2);
        }
        digitalWrite(config.CS, 1);
        pthread_mutex_unlock(&screenLock);

    }



    void Display::showImage(Image image) {
        showImage(image, DEGREE_0);
    }




    void Display::setScreenWindow(_word x1, _word y1, _word x2, _word y2) {
        writeCommand(0x2a);
        writeByte(x1 >> 8);
        writeByte(x1 & 0xff);
        writeByte((x2 - 1) >> 8);
        writeByte((x2 - 1) & 0xff);

        writeCommand(0x2b);
        writeByte(y1 >> 8);
        writeByte(y1 & 0xff);
        writeByte((y2 - 1) >> 8);
        writeByte((y2 - 1) & 0xff);

        writeCommand(0x2c);
    }

    void Display::writeCommand(_byte data) {
        digitalWrite(config.CS, 0);
        digitalWrite(config.DC, 0);
        wiringPiSPIDataRW(0, &data, 1);
    }

    void Display::writeByte(_byte data) {
        digitalWrite(config.CS, 0);
        digitalWrite(config.DC, 1);
        wiringPiSPIDataRW(0, &data, 1);
        digitalWrite(config.CS, 1);
    }

    void Display::writeBytes(_byte* data, uint32_t len) {
        _byte d2[len];

        memcpy(&d2, data, len); // wpi write back to buffer!

        wiringPiSPIDataRW(0, d2, len);
    }

    void Display::pause() {
        digitalWrite(config.CS, 1);
        digitalWrite(config.DC, 1);
    }

    void Display::resume() {
        digitalWrite(config.CS, 0);
    }


    /*
    void Display::writeWord(_word data) {
        digitalWrite(config.CS, 0);
        digitalWrite(config.DC, 1);

        _byte b1 = (data >> 8) & 0xff;
        wiringPiSPIDataRW(0, &b1, 1);

        _byte b2 = data & 0x00ff;
        wiringPiSPIDataRW(0, &b2, 1);


        writeByte(data);
        digitalWrite(config.CS, 1);
    }
    */

}