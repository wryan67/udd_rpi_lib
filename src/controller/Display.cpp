#include "Display.h"
#include "Image.h"
#include "IdGenerator.h"

#include <unistd.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

namespace udd {
    std::recursive_mutex screenLock;

    Display::Display() {
        displayId = IdGenerator::next(); 
    }



    void Display::openDisplay(DisplayConfigruation configuration) {
        this->config = configuration;
        this->vImage = Image(config.width, config.height, BLACK);

        openSPI();

        pinMode(config.CS,  OUTPUT);
        pinMode(config.RST, OUTPUT);
        pinMode(config.DC,  OUTPUT);
        if (config.BLK >= 0) {
            pinMode(config.BLK, OUTPUT);
        }
        if (config.busyPin>= 0) {
            pinMode(config.busyPin, INPUT);
        }

        visable();

        init();
        pause();


    }

    void Display::reset() {
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
        printf("displayId:         %d\n", displayId);
        printf("width:             %d\n", config.width);
        printf("height:            %d\n", config.height);
        printf("xOffset:           %d\n", config.xOffset);
        printf("yOffset:           %d\n", config.yOffset);

        printf("mirror:            %d\n", config.screenMirror);
        printf("rotation:          %d\n", config.screenRotation);

        printf("cs:                %d\n", config.CS);
        printf("dc:                %d\n", config.DC);
        printf("rst:               %d\n", config.RST);
        printf("blk:               %d\n", config.BLK);
        printf("busy pin:          %d\n", config.busyPin);

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
        if (config.BLK >= 0) {
            digitalWrite(config.BLK, 1);
        }
    }

    void Display::hidden() {
        if (config.BLK >= 0) {
            digitalWrite(config.BLK, 0);
        }
    }

    void Display::clear(Color color) {
        screenLock.lock();

        openSPI();
        resume();

        ColorType ct = color.toType();

        _word  row[config.width+config.xOffset];
        _byte* rowPointer = (_byte*)(row);
        _word  cx = color2word(&ct);

        int width = config.width + config.xOffset;
        int height = config.height + config.yOffset;

        for (int x = 0; x < width; x++) {
            row[x] = cx;
        }

        setWindow(0, 0, width, height, DEGREE_0);
        digitalWrite(config.DC, 1);
        digitalWrite(config.CS, 0);

        for (int y = 0; y < height; y++) {
            writeData(rowPointer, (width) * 2);
        }
        pause();
        screenLock.unlock();
    }
    
    void Display::printRotation(Rotation rotation) {
        fprintf(stderr, "Rotation:  %3d\n",rotation);
        fflush(stderr);
    }

    void Display::showImage(Image &image, Rotation rotation) {

        screenLock.lock();
        openSPI();
        resume();

//        int width = config.width + config.xOffset;
//        int height = config.height + config.yOffset;
        int width = windowP2.x - windowP1.x + 1;
        int height= windowP2.y - windowP1.y + 1;

        printRotation(rotation);
        fprintf(stderr, "showImage configW=%4d  configY=%4d\n", config.width, config.height);
        fprintf(stderr, "showImage xOffset=%4d  yOffset=%4d\n", config.xOffset, config.yOffset);
        fprintf(stderr, "showImage width=  %4d  height= %4d\n", width, height); 
        fflush(stderr);


        setWindow(windowP1.x, windowP1.y, windowP2.x, windowP2.y);
        digitalWrite(config.DC, 1);
        digitalWrite(config.CS, 0);

        _word  row[width];
        _byte* rowPointer = (_byte*)(row);

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; ++x) {
                ColorType* ct = image.getPixel(x, y, rotation);

                if (ct == NULL) {
                    row[x] = 0;
                } else {
                    row[x] = color2word(ct);
                }
            }

            writeData(rowPointer, (width) * 2);
        }
        digitalWrite(config.CS, 1);

        pause();
        screenLock.unlock();
    }




    void Display::showImage(Image &image) {
        showImage(image, DEGREE_0);
    }

    void Display::setWindowFullScreen() {
        setWindow(0, 0, config.width-1 + config.xOffset, config.height-1 + config.yOffset, config.screenRotation);
    }


    void Display::setWindow(int x1, int y1, int x2, int y2) {
        windowP1.x = x1;
        windowP1.y = y1;
        windowP2.x = x2;
        windowP2.y = y2;
    }

    void Display::adjustPoint(int &x, int &y, Rotation rotation) {

        int tmpX = x;
        int tmpY = y;

        switch (rotation) {
        case DEGREE_0:    return;

        case DEGREE_90:   x = y;  
                          y = config.height - tmpX - 1; 
                          return;

        case DEGREE_180:  x = config.width - tmpX - 1; 
                          y = config.height - tmpY - 1; 
                          return;

        case DEGREE_270:  x = config.width - tmpY - 1;
                          y = tmpX;
                          return;

//        case DEGREE_0:    return getPixelColor(x,             y);
//        case DEGREE_90:   return getPixelColor(y,             height - x - 1);
//        case DEGREE_180:  return getPixelColor(width - x - 1, height - y - 1);
//        case DEGREE_270:  return getPixelColor(width - y - 1, x);

        default:
            fprintf(stderr, "not implemented yet, rotation degree=%d", rotation);
            return;
        }

    }

    void Display::setWindow(int x1, int y1, int x2, int y2, Rotation rotation) {
        windowP1.x = x1;
        windowP1.y = y1;
        windowP2.x = x2;
        windowP2.y = y2;
    }


    void Display::writeCommand(_byte data) {
        digitalWrite(config.DC, 0);
        digitalWrite(config.CS, 0);
        wiringPiSPIDataRW(0, &data, 1);
        digitalWrite(config.CS, 1);
    }

    void Display::writeData(_byte data) {
        digitalWrite(config.DC, 1);
        digitalWrite(config.CS, 0);
        wiringPiSPIDataRW(0, &data, 1);
        digitalWrite(config.CS, 1);
    }

    void Display::writeData(_byte* data, uint32_t len) {
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

   void Display::setPixel(Pixel pixel) {
       this->vImage.drawPoint(pixel.point.x, pixel.point.y, pixel.color,1);
   }

}