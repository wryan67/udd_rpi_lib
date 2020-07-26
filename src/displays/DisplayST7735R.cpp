#include "DisplayST7735R.h"
#include <wiringPi.h>

namespace udd {
    LCD_DIS sLCD_DIS;

    DisplayST7735R::DisplayST7735R() : Display() {}
     
    void DisplayST7735R::init()  {
        reset();

        printf("init st7735\n");

        writeCommand(0x36);
        writeData(0x00);

        writeCommand(0x3A);
        writeData(0x05);

        writeCommand(0x21);

        writeCommand(0x2A);
        writeData(0x00);
        writeData(0x00);
        writeData(0x01);
        writeData(0x3F);

        writeCommand(0x2B);
        writeData(0x00);
        writeData(0x00);
        writeData(0x00);
        writeData(0xEF);

        writeCommand(0xB2);
        writeData(0x0C);
        writeData(0x0C);
        writeData(0x00);
        writeData(0x33);
        writeData(0x33);

        writeCommand(0xB7);
        writeData(0x35);

        writeCommand(0xBB);
        writeData(0x1F);

        writeCommand(0xC0);
        writeData(0x2C);

        writeCommand(0xC2);
        writeData(0x01);

        writeCommand(0xC3);
        writeData(0x12);

        writeCommand(0xC4);
        writeData(0x20);

        writeCommand(0xC6);
        writeData(0x0F);

        writeCommand(0xD0);
        writeData(0xA4);
        writeData(0xA1);

        writeCommand(0xE0);
        writeData(0xD0);
        writeData(0x08);
        writeData(0x11);
        writeData(0x08);
        writeData(0x0C);
        writeData(0x15);
        writeData(0x39);
        writeData(0x33);
        writeData(0x50);
        writeData(0x36);
        writeData(0x13);
        writeData(0x14);
        writeData(0x29);
        writeData(0x2D);

        writeCommand(0xE1);
        writeData(0xD0);
        writeData(0x08);
        writeData(0x10);
        writeData(0x08);
        writeData(0x06);
        writeData(0x06);
        writeData(0x39);
        writeData(0x44);
        writeData(0x51);
        writeData(0x0B);
        writeData(0x16);
        writeData(0x14);
        writeData(0x2F);
        writeData(0x31);
        writeCommand(0x21);

        writeCommand(0x11);

        writeCommand(0x29);

    }
    void DisplayST7735R::setWindow(int x1, int y1, int x2, int y2, Rotation rotation) {
        Display::setWindow(x1, y1, x2, y2);

        adjustPoint(x1, y1, rotation);
        adjustPoint(x2, y2, rotation);

        writeCommand(0x2a);
        writeData(x1 >> 8);
        writeData(x1 & 0xff);
        writeData((x2 - 1) >> 8);
        writeData((x2 - 1) & 0xff);

        writeCommand(0x2b);
        writeData(y1 >> 8);
        writeData(y1 & 0xff);
        writeData((y2 - 1) >> 8);
        writeData((y2 - 1) & 0xff);

        writeCommand(0x2C);
    }
    void DisplayST7735R::initScanDir(LCD_SCAN_DIR Scan_dir) {
        //Get the screen scan direction
        sLCD_DIS.LCD_Scan_Dir = Scan_dir;

        //Get GRAM and LCD width and height
        if (Scan_dir == L2R_U2D || Scan_dir == L2R_D2U || Scan_dir == R2L_U2D || Scan_dir == R2L_D2U) {
            sLCD_DIS.LCD_Dis_Column = config.height;
            sLCD_DIS.LCD_Dis_Page = config.width;
        }
        else {
            sLCD_DIS.LCD_Dis_Column = config.width;
            sLCD_DIS.LCD_Dis_Page = config.height;
        }

        // Gets the scan direction of GRAM
        uint16_t MemoryAccessReg_Data = 0;  //0x36

        switch (Scan_dir) {
        case L2R_U2D:
            MemoryAccessReg_Data = 0X00 | 0x00;//x Scan direction | y Scan direction
            break;
        case L2R_D2U:
            MemoryAccessReg_Data = 0x00 | 0x80;//0xC8 | 0X10
            break;
        case R2L_U2D://	0X4
            MemoryAccessReg_Data = 0x40 | 0x00;
            break;
        case R2L_D2U://	0XC
            MemoryAccessReg_Data = 0x40 | 0x80;
            break;
        case U2D_L2R://0X2
            MemoryAccessReg_Data = 0X00 | 0X00 | 0x20;
            break;
        case U2D_R2L://0X6
            MemoryAccessReg_Data = 0x00 | 0X40 | 0x20;
            break;
        case D2U_L2R://0XA
            MemoryAccessReg_Data = 0x80 | 0x00 | 0x20;
            break;
        case D2U_R2L://0XE
            MemoryAccessReg_Data = 0x40 | 0x80 | 0x20;
            break;
        }

        //please set (MemoryAccessReg_Data & 0x10) != 1
        if ((MemoryAccessReg_Data && 0x20) != 1) {
            sLCD_DIS.LCD_X_Adjust = LCD_X;
            sLCD_DIS.LCD_Y_Adjust = LCD_Y;
        }
        else {
            sLCD_DIS.LCD_X_Adjust = LCD_Y;
            sLCD_DIS.LCD_Y_Adjust = LCD_X;
        }

        // Set the read / write scan direction of the frame memory
        writeCommand(0x36); //MX, MY, RGB mode
#if defined(LCD_1IN44)
        writeData(MemoryAccessReg_Data | 0x08);	//0x08 set RGB
#elif defined(LCD_1IN8)
        LCD_WriteData_8Bit(MemoryAccessReg_Data & 0xf7);	//RGB color filter panel
#endif


    }

    _word DisplayST7735R::color2word(ColorType* xp) {
        double bluePct = xp->blue / 255.0;
        double greenPct = xp->green / 255.0;
        double redPct = xp->red / 255.0;

        int red = redPct * 0x1f;
        int green = greenPct * 0x3f;
        int blue = bluePct * 0x1f;

        _word buf = ((0x1f & (~blue)) << 11) | ((0x3f & (~green)) << 5) | ((0x1f & (~red)));

        return (buf >> 8) | buf << 8;
        //return buf;
    }
}
