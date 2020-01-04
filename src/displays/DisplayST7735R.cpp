#include "DisplayST7735R.h"
#include <wiringPi.h>

namespace udd {
    LCD_DIS sLCD_DIS;

    DisplayST7735R::DisplayST7735R() : Display() {}
     
    void DisplayST7735R::init()  {
        reset();

        printf("init st7735\n");

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
        writeByte(MemoryAccessReg_Data | 0x08);	//0x08 set RGB
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
