#include "DisplayST7789.h"


namespace udd {

    DisplayST7789::DisplayST7789() {}

    DisplayST7789::DisplayST7789(DisplayConfigruation config) : Display(config) {
        this->config.displayType = ST7789;
    }
     
    void DisplayST7789::init() {
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
}
