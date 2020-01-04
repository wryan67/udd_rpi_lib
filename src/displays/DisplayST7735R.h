#pragma once
#include <Display.h>

namespace udd {
#define	POINT				uint16_t		//The type of coordinate (unsigned short) 
#define	LENGTH				uint16_t		//The type of coordinate (unsigned short) 
#define LCD_1IN44
    //#define LCD_1IN8
#if defined(LCD_1IN44)
#define LCD_WIDTH  128  //LCD width
#define LCD_HEIGHT  128 //LCD height
#define LCD_X	2
#define LCD_Y	1
#elif defined(LCD_1IN8)
#define LCD_WIDTH  160  //LCD width
#define LCD_HEIGHT  128 //LCD height
#define LCD_X	 1
#define LCD_Y	 2
#endif

    typedef enum {
        L2R_U2D = 0,	//The display interface is displayed , left to right, up to down 
        L2R_D2U,
        R2L_U2D,
        R2L_D2U,

        U2D_L2R,
        U2D_R2L,
        D2U_L2R,
        D2U_R2L,
    }LCD_SCAN_DIR;
//#define SCAN_DIR_DFT  U2D_R2L  //Default scan direction = L2R_U2D

#define SCAN_DIR_DFT  D2U_R2L  //Default scan direction = L2R_U2D


    typedef struct {
        LENGTH LCD_Dis_Column;	//COLUMN
        LENGTH LCD_Dis_Page;	//PAGE
        LCD_SCAN_DIR LCD_Scan_Dir;
        POINT LCD_X_Adjust;		//LCD x actual display position calibration
        POINT LCD_Y_Adjust;		//LCD y actual display position calibration
    }LCD_DIS;

    extern LCD_DIS sLCD_DIS;



    class DisplayST7735R : public Display {
    public:
        DisplayST7735R();

        void init() override;
        void initScanDir(LCD_SCAN_DIR direction);

        _word color2word(ColorType* xp);

    };
}
