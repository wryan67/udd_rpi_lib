#include "Display.h"
#include "Image.h"
#include "IdGenerator.h"

#include <unistd.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

namespace udd {
#define swap(t, a, b)  {t tmp=a; a=b; b=tmp;}

    std::recursive_mutex screenLock;

    Display::Display() {
        displayId = IdGenerator::next(); 
    }

    void Display::init() {
        fprintf(stderr, "init failed, base method called instead of child\n");
        exit(0);
    }



    void Display::openDisplay(DisplayConfigruation configuration) {
        this->config = configuration;
//        this->vImage = Image(config.width, config.height, BLACK);

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

    void Display::clearWindow(Color color, Point p1, Point p2, Rotation rotation) {
        screenLock.lock();

        openSPI();
        resume();

        ColorType ct = color.toType();

        _word  row[config.width + config.xOffset];
        _byte* rowPointer = (_byte*)(row);
        _word  cx = color2word(&ct);

        int width = p2.x - p1.x + 1;
        int height = p2.y - p1.y + 1;

        for (int x = 0; x < width; x++) {
            row[x] = cx;
        }

        setWindow(p1,p2, DEGREE_0);
        digitalWrite(config.DC, 1);
        digitalWrite(config.CS, 0);

        for (int y = 0; y < height; y++) {
            writeData(rowPointer, (width) * 2);
        }
        pause();
        screenLock.unlock();
    }

    void Display::clearScreen(Color color) {
        printf("clearScreen: starts\n"); fflush(stdout);

        
        screenLock.lock();


        openSPI();
        resume();

        int width = config.width + config.xOffset;
        int height = config.height + config.yOffset;

        printf("clearScreen: width=%d height=%d\n", width, height); fflush(stdout);

        _word  row[width];
        _byte* rowPointer = (_byte*)(row);

        ColorType ct = color.toType();
        _word  cx = color2word(&ct);

        for (int x = 0; x < width; x++) {
            row[x] = cx;
        }

        printf("clearScreen: tag01\n"); fflush(stdout);
        setWindow(Point(0, 0), Point(width-1, height-1), DEGREE_0);

        digitalWrite(config.DC, 1);
        digitalWrite(config.CS, 0);

        printf("clearScreen: write data row width=%d\n", width); fflush(stdout);

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


    void Display::showImage(Image& image) {
        showImage(image, Point(0,0), Point(config.width,config.height), DEGREE_0);
    }

    void Display::showImage(Image& image, Rotation rotation) {
        Point p1 = Point(0, 0);
        Point p2 = Point(config.width, config.height);

        if (image.getWidth() > config.width) {
            p2 = Point(config.height, config.width);
        }
        showImage(image, p1, p2, rotation);
    }
    
    void Display::showImage(Image& image, Point p1, Point p2, Rotation rotation) {
        int width, height;
        screenLock.lock();
        openSPI();
        resume();


        setWindow(p1, p2, rotation);

        width = p2.x - p1.x + 1;
        height = p2.y - p1.y + 1;

        if (image.getHeight() != height ||
            image.getWidth() != width) {
            fprintf(stderr, "Image size does not match window size. Height: [Image=%d, Window=%d]   Width: [Image=%d, Window=%d]\n",
                image.getHeight(), height, image.getWidth(), width);
        }

/*
        switch (rotation) {
        case DEGREE_0:
        case DEGREE_180:
            width = windowP2.x - windowP1.x + 1;
            height = windowP2.y - windowP1.y + 1;
            break;
        case DEGREE_90:
        case DEGREE_270:
            height = windowP2.x - windowP1.x + 1;
            width  = windowP2.y - windowP1.y + 1;
            break;
        default:
            width = 0;
            height = 0;
        }
*/

        printRotation(rotation);
        fprintf(stderr, "showImage configW=%4d  configY=%4d\n", config.width, config.height);
        fprintf(stderr, "showImage xOffset=%4d  yOffset=%4d\n", config.xOffset, config.yOffset);
        fprintf(stderr, "showImage width=  %4d  height= %4d\n", width, height); 
        fflush(stderr);


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





    void Display::setWindow(Point p1, Point p2, Rotation rotation) {

        int x1 = p1.x;
        int y1 = p1.y;

        int x2 = p2.x;
        int y2 = p2.y;

        fprintf(stderr, "p1(%3d,%3d) p2(%3d,%3d)\n", x1, y1, x2, y2);

        adjustPoint(x1, y1, rotation);
        adjustPoint(x2, y2, rotation);

        fprintf(stderr, "p2(%3d,%3d) p2(%3d,%3d)\n", x1, y1, x2, y2);


        switch (rotation) {
        case DEGREE_0: {     
                            break;
        }
        case DEGREE_90: {    
                            swap(int, x1, x2);
                            break;
        }
        case DEGREE_180: {   
                            swap(int, x1, x2);
                            swap(int, y1, y2);
                            break;
        }
        case DEGREE_270: {
                            swap(int, y1, y2);
                            break;
        }
        }


        fprintf(stderr, "p4(%3d,%3d) p2(%3d,%3d)\n", x1, y1, x2, y2);
        fflush(stderr);

        writeCommand(0x2a);    // caset   x1 <= y2
        writeData(x1 >> 8);
        writeData(x1 & 0xff);
        writeData((x2) >> 8);
        writeData((x2) & 0xff);

        writeCommand(0x2b);     // raset   y1 <= y2
        writeData(y1 >> 8);
        writeData(y1 & 0xff);
        writeData((y2) >> 8);
        writeData((y2) & 0xff);

        writeCommand(0x2C);

    }

    void Display::adjustPoint(int &x, int &y, Rotation rotation) {

        int tmpX = x;
        int tmpY = y;

        switch (rotation) {
        case DEGREE_0:      return;

        case DEGREE_90:     x = config.width - tmpY - 1;
                            y = tmpX;
                            return;

        case DEGREE_180:    x = config.width - tmpX - 1; 
                            y = config.height - tmpY - 1; 
                            return;

        case DEGREE_270:    x = tmpY;
                            y = config.height - tmpX -1;
                            return;

        default:
            fprintf(stderr, "not implemented yet, rotation degree=%d", rotation);
            return;
        }

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

    /*
   void Display::setPixel(Pixel pixel) {
       this->vImage.drawPoint(pixel.point.x, pixel.point.y, pixel.color,1);
   }
   */

}