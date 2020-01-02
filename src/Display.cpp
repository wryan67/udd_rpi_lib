#include "Display.h"

#include <unistd.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>

Display::Display(DisplayConfigruation config) {
    this->config = config;
    openDisplay();
}


void Display::openDisplay() {
    openSPI();

    pinMode(config.CS,   OUTPUT);
    pinMode(config.RST,  OUTPUT);
    pinMode(config.DC,   OUTPUT);
    pinMode(config.BLK,  OUTPUT);

    visable();

    init();
}

void Display::reset() {
    digitalWrite(config.CS, 1);
    usleep(100*1000);
    digitalWrite(config.RST, 0);
    usleep(100*1000);
    digitalWrite(config.RST, 1);
    usleep(100 * 1000);
}

_word Display::color2word(ColorType *xp) {
    return ((0x1f & (xp->blue)) << 11) | ((0x3f & (xp->red)) << 5) | ((0x1f & (xp->green)));
}

void Display::init() {
    reset();

    writeCommand(0x36);
    writeByte(0x00);

    writeCommand(0x3A);
    writeByte(0x05);

    writeCommand(0x21);

    writeCommand(0x2A);
    writeByte(0x00);
    writeByte(0x00);
    writeByte(0x01);
    writeByte(0x3F);

    writeCommand(0x2B);
    writeByte(0x00);
    writeByte(0x00);
    writeByte(0x00);
    writeByte(0xEF);

    writeCommand(0xB2);
    writeByte(0x0C);
    writeByte(0x0C);
    writeByte(0x00);
    writeByte(0x33);
    writeByte(0x33);

    writeCommand(0xB7);
    writeByte(0x35);

    writeCommand(0xBB);
    writeByte(0x1F);

    writeCommand(0xC0);
    writeByte(0x2C);

    writeCommand(0xC2);
    writeByte(0x01);

    writeCommand(0xC3);
    writeByte(0x12);

    writeCommand(0xC4);
    writeByte(0x20);

    writeCommand(0xC6);
    writeByte(0x0F);

    writeCommand(0xD0);
    writeByte(0xA4);
    writeByte(0xA1);

    writeCommand(0xE0);
    writeByte(0xD0);
    writeByte(0x08);
    writeByte(0x11);
    writeByte(0x08);
    writeByte(0x0C);
    writeByte(0x15);
    writeByte(0x39);
    writeByte(0x33);
    writeByte(0x50);
    writeByte(0x36);
    writeByte(0x13);
    writeByte(0x14);
    writeByte(0x29);
    writeByte(0x2D);

    writeCommand(0xE1);
    writeByte(0xD0);
    writeByte(0x08);
    writeByte(0x10);
    writeByte(0x08);
    writeByte(0x06);
    writeByte(0x06);
    writeByte(0x39);
    writeByte(0x44);
    writeByte(0x51);
    writeByte(0x0B);
    writeByte(0x16);
    writeByte(0x14);
    writeByte(0x2F);
    writeByte(0x31);
    writeCommand(0x21);

    writeCommand(0x11);

    writeCommand(0x29);
}

void Display::printConfiguration() {
    printf("width:             %d\n", config.width);
    printf("height:            %d\n", config.height);
    printf("xOffset:           %d\n", config.xOffset);
    printf("yOffset:           %d\n", config.yOffest);
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
    handle = wiringPiSPISetup(config.spiChannel, config.spiSpeed);
}

void Display::visable() {
    digitalWrite(config.BLK, 1);
}

void Display::hidden() {
    digitalWrite(config.BLK, 0);
}

void Display::clear(Color color) {
    ColorType ct = color.toType();

    _word  row[config.width];
    _byte* rowPointer = (_byte*)(row);
    _word  cx = color2word(&ct);

    for (int x = 0; x < config.width; x++) {
        row[x] = cx;
    }

    setScreenWindow(0, 0, config.width, config.height);
    digitalWrite(config.DC, 1);

    for (int y = 0; y < config.height; y++) {
        writeBytes(rowPointer, config.width * 2);
    }
}


void Display::showImage(Image image) {
    printf("tag21\n"); fflush(stdout);

    setScreenWindow(0, 0, config.width, config.height);
    digitalWrite(config.DC, 1);

    _word  row[config.width];
    _byte* rowPointer = (_byte*)(row);

    for (int y = 0; y < config.height; y++) {       
        for (int x = 0; x < config.width; ++x) {
            ColorType *ct = image.getPixel(x, y);
            row[x] = color2word(ct);
        }

        writeBytes(rowPointer, config.width * 2);
    }
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

