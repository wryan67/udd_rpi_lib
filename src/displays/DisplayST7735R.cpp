#include "DisplayST7735R.h"
#include <wiringPi.h>

namespace udd {
    LCD_DIS sLCD_DIS;

    DisplayST7735R::DisplayST7735R() : Display() {}
    DisplayST7735R::DisplayST7735R(DisplayConfigruation config) : Display(config) {
        this->config.displayType = ST7735;
    }
     
    void DisplayST7735R::init() {
        reset();

        //ST7735R Frame Rate
        writeCommand(0xB1);
        writeByte(0x01);
        writeByte(0x2C);
        writeByte(0x2D);

        writeCommand(0xB2);
        writeByte(0x01);
        writeByte(0x2C);
        writeByte(0x2D);

        writeCommand(0xB3);
        writeByte(0x01);
        writeByte(0x2C);
        writeByte(0x2D);
        writeByte(0x01);
        writeByte(0x2C);
        writeByte(0x2D);

        writeCommand(0xB4); //Column inversion
        writeByte(0x07);

        //ST7735R Power Sequence
        writeCommand(0xC0);
        writeByte(0xA2);
        writeByte(0x02);
        writeByte(0x84);
        writeCommand(0xC1);
        writeByte(0xC5);

        writeCommand(0xC2);
        writeByte(0x0A);
        writeByte(0x00);

        writeCommand(0xC3);
        writeByte(0x8A);
        writeByte(0x2A);
        writeCommand(0xC4);
        writeByte(0x8A);
        writeByte(0xEE);

        writeCommand(0xC5); //VCOM
        writeByte(0x0E);

        //ST7735R Gamma Sequence
        writeCommand(0xe0);
        writeByte(0x0f);
        writeByte(0x1a);
        writeByte(0x0f);
        writeByte(0x18);
        writeByte(0x2f);
        writeByte(0x28);
        writeByte(0x20);
        writeByte(0x22);
        writeByte(0x1f);
        writeByte(0x1b);
        writeByte(0x23);
        writeByte(0x37);
        writeByte(0x00);
        writeByte(0x07);
        writeByte(0x02);
        writeByte(0x10);

        writeCommand(0xe1);
        writeByte(0x0f);
        writeByte(0x1b);
        writeByte(0x0f);
        writeByte(0x17);
        writeByte(0x33);
        writeByte(0x2c);
        writeByte(0x29);
        writeByte(0x2e);
        writeByte(0x30);
        writeByte(0x30);
        writeByte(0x39);
        writeByte(0x3f);
        writeByte(0x00);
        writeByte(0x07);
        writeByte(0x03);
        writeByte(0x10);

        writeCommand(0xF0); //Enable test command
        writeByte(0x01);

        writeCommand(0xF6); //Disable ram power save mode
        writeByte(0x00);

        writeCommand(0x3A); //65k mode
        writeByte(0x05);


        LCD_SCAN_DIR LCD_ScanDir = SCAN_DIR_DFT;//SCAN_DIR_DFT = D2U_L2R
        initScanDir(LCD_ScanDir);

        delay(200);

        //sleep out
        writeCommand(0x11);
        delay(120);

        //Turn on the LCD display
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
