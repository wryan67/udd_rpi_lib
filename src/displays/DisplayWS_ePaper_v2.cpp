#include "DisplayWS_ePaper_v2.h"
#include <wiringPi.h>

namespace udd {

    DisplayWS_ePaper_v2::DisplayWS_ePaper_v2() : Display() {}

    void DisplayWS_ePaper_v2::init() {
        reset();

        writeCommand(0x04);
        readBusy();

        printf("init\n"); fflush(stdout);

        writeCommand(0x00);//panel setting
        writeData(0x0f);//LUT from OTP?128x296
        writeData(0x89);//Temperature sensor, boost and other related timing settings

        writeCommand(0x61);//resolution setting
        writeData(0x80);
        writeData(0x01);
        writeData(0x28);

        writeCommand(0X50);//VCOM AND DATA INTERVAL SETTING			
        writeData(0x77);//WBmode:VBDF 17|D7 VBDW 97 VBDB 57

    }

    void DisplayWS_ePaper_v2::readBusy(void) {
        printf("readBusy pin=%d\n", config.busyPin); fflush(stdout);
        unsigned char busy;
        do {
            writeCommand(0x71);
            busy = digitalRead(config.busyPin);
            busy = !(busy & 0x01);
        } while (busy);
        delay(200);
    }


    void DisplayWS_ePaper_v2::reset() {
        printf("reset pin=%d\n", config.RST); fflush(stdout);
        digitalWrite(config.DC, 1);
        digitalWrite(config.CS, 1);
        digitalWrite(config.RST, 1);
        delay(200);
        digitalWrite(config.RST, 0);
        delay(10);
        digitalWrite(config.RST, 1);
        delay(200);
    }


    
    void DisplayWS_ePaper_v2::clear(Color color) {
        screenLock.lock();

        openSPI();
        resume();

        char colorName[12];

        int _color=-1;
        int width = (config.width % 8 == 0) ? (config.width / 8) : (config.width / 8 + 1);

        if (color.equals(WHITE)) {
             _color = 0;
             strcpy(colorName, "white");
         } else if (color.equals(BLACK)) {
             _color = 1;
             strcpy(colorName, "black");
        } else if (color.equals(RED)) {
            _color = 2;
            strcpy(colorName, "red");
        } else {
             fprintf(stderr, "ePaper.clear: unknown color");
             return;
        }

        writeCommand(0x10);  // black/white
        for (int y = 0; y < config.height; ++y) {
            for (int x = 0; x < width; ++x) {
                switch (_color) {
                case 0:  writeData(0xff);  //white
                            break;
                case 1:  writeData(0x00);  //black
                            break;
                case 2:  writeData(0xff);  //red
                    break;
                }
            }
        }
        
        writeCommand(0x92);  // red begin
        writeCommand(0x13);

        for (int y = 0; y < config.height; ++y) {
            for (int x = 0; x < width; ++x) {
                switch (_color) {
                case 0:  writeData(0xff);  //white
                    break;
                case 1:  writeData(0xff);  //black
                    break;
                case 2:  writeData(0x00);  //red
                    break;
                }
            }
        }
        writeCommand(0x92); // red end
        writeCommand(0x12);
        readBusy();     

        printf("ePaper clear %s\n", colorName);
        
        pause();
        screenLock.unlock();
    }

    void DisplayWS_ePaper_v2::showImage(Image image) {
        showImage(image, DEGREE_0);
    }

    void addBit(int bit, _byte* byte, int val) {
        if (val>0) {
          (*byte) = (*byte) | (val << (7-bit));
        }
    }


    void DisplayWS_ePaper_v2::showImage(Image image, Rotation rotation) {

        screenLock.lock();
        openSPI();

        fprintf(stderr, "ePaper showImage(%d,%d)\n", config.width, config.height);

//        int writeBytes = (config.width % 8 == 0) ? (config.width / 8) : (config.width / 8 + 1);
        int width = config.width + config.xOffset;
        int height = config.height + config.yOffset;

//        setScreenWindow(0, 0, width, height);
        digitalWrite(config.DC, 1);
        digitalWrite(config.CS, 0);


        writeCommand(0x10);  // black/white

        for (int y = 0; y < height; y++) {
            int  bits = 0;
            _byte out = 0;
            for (int x = 0; x < width; ++x) {
                ColorType* ct = image.getPixel(x - config.xOffset, y - config.yOffset, rotation);
                int val=1;
                if (ct != NULL) {
                    if (WHITE.equals(ct)) {
                        val=1;
                    }
                    else if (BLACK.equals(ct)) {
                        val=0;
                    }
                    else if (RED.equals(ct)) {
                        val=1;
                    } else {
                        fprintf(stderr, "invalid color found at (%d,%d)\n", x, y);
                    }
                }
                if (++bits % 8 == 0) {
                    writeData(out);
                    out=0;
                }
                addBit(bits%8, &out, val);
            }
            if (width%8 != 0) {
                writeData(out);
            }
        }

        writeCommand(0x92);  // red begin
        writeCommand(0x13);

        for (int y = 0; y < height; y++) {
            int  bits = 0;
            _byte out = 0;
            for (int x = 0; x < width; ++x) {
                ColorType* ct = image.getPixel(x - config.xOffset, y - config.yOffset, rotation);
                int val=1;
                if (ct != NULL) {
                    if (WHITE.equals(ct)) {
                        val=1;
                    }
                    else if (BLACK.equals(ct)) {
                        val=1;
                    }
                    else if (RED.equals(ct)) {
                        val=0;
                    } else {
                        fprintf(stderr, "invalid color found at (%d,%d)\n", x, y);
                    }
                }
                if (++bits % 8 == 0) {
                    writeData(out);
                    out=0;
                }
                addBit(bits%8, &out, val);
            }
            if (width%8 != 0) {
                writeData(out);
            }
        }

        writeCommand(0x92); // red end
        writeCommand(0x12);
        readBusy();

        digitalWrite(config.CS, 1);
        screenLock.unlock();
    }

    void DisplayWS_ePaper_v2::setPartReg() {

    /**
     * partial screen update LUT
    **/
    const unsigned char EPD_2IN9D_lut_vcom1[] = {
        0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00
        ,0x00, 0x00,
    };
    const unsigned char EPD_2IN9D_lut_ww1[] = {
        0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    const unsigned char EPD_2IN9D_lut_bw1[] = {
        0x80, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    const unsigned char EPD_2IN9D_lut_wb1[] = {
        0x40, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };
    const unsigned char EPD_2IN9D_lut_bb1[] = {
        0x00, 0x19, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };



        writeCommand(0x82);      //vcom_DC setting
        writeData(0x00);
        writeCommand(0X50);
        writeData(0xb7);

        unsigned int count;
        writeCommand(0x20);
        for(count=0; count<44; count++) {
            writeData(EPD_2IN9D_lut_vcom1[count]);
        }

        writeCommand(0x21);
        for(count=0; count<42; count++) {
            writeData(EPD_2IN9D_lut_ww1[count]);
        }

        writeCommand(0x22);
        for(count=0; count<42; count++) {
            writeData(EPD_2IN9D_lut_bw1[count]);
        }

        writeCommand(0x23);
        for(count=0; count<42; count++) {
            writeData(EPD_2IN9D_lut_wb1[count]);
        }

        writeCommand(0x24);
        for(count=0; count<42; count++) {
            writeData(EPD_2IN9D_lut_bb1[count]);
        }
    }
}