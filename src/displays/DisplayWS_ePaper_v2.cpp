#include "DisplayWS_ePaper_v2.h"
#include "DisplayWS_ePaper_v2_LUT.h"
#include <wiringPi.h>
#include <unistd.h>

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

    void DisplayWS_ePaper_v2::initPartial() {
        // printf("reset\n"); fflush(stdout);
        // reset();
        // printf("init\n"); fflush(stdout);

        // writeCommand(0x01);	//POWER SETTING
        // writeData(0x03);
        // writeData(0x00);
        // writeData(0x2b);
        // writeData(0x2b);
        // writeData(0x03);

        // writeCommand(0x06);	//boost soft start
        // writeData(0x17);     //A
        // writeData(0x17);     //B
        // writeData(0x17);     //C

        // writeCommand(0x04);
        // readBusy();

        writeCommand(0x00);	//panel setting
        writeData(0xbf);     //LUT from OTP，128x296
        writeData(0x0e);     //VCOM to 0V fast

        writeCommand(0x30);	//PLL setting
        writeData(0x3a);     // 3a 100HZ   29 150Hz 39 200HZ	31 171HZ


        writeCommand(0x61);  //resolution setting
        writeData(config.width);
        writeData((config.height >> 8) & 0xff);
        writeData(config.height & 0xff);

        writeCommand(0X82);//VCOM AND DATA INTERVAL SETTING			
        writeData(0x28);

    }

    void DisplayWS_ePaper_v2::readBusy(void) {
        unsigned char busy;
        do {
            writeCommand(0x71);
            busy = digitalRead(config.busyPin);
            busy = !(busy & 0x01);
        } while (busy);
        delay(200);
    }


    void DisplayWS_ePaper_v2::reset() {
        digitalWrite(config.DC, 1);
        digitalWrite(config.CS, 1);
        digitalWrite(config.RST, 1);
        delay(200);
        digitalWrite(config.RST, 0);
        delay(200);
        digitalWrite(config.RST, 1);
        delay(200);
    }

    void DisplayWS_ePaper_v2::enableDisplay() {
        writeCommand(0x12);
        usleep(210);
        readBusy();
    }

    
    void DisplayWS_ePaper_v2::clearScreen(Color color) {
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


        EPD_SetFullReg();
        enableDisplay();    
        pause();
        screenLock.unlock();
    }



    void addBit(int bit, _byte* byte, int val) {
        if (val>0) {
          (*byte) = (*byte) | (val << (7-bit));
        }
    }

    void DisplayWS_ePaper_v2::showImage(Image& image) {
        Display::showImage(image);
    }

    void DisplayWS_ePaper_v2::showImage(Image& image, Rotation rotation) {
        Display::showImage(image, rotation);
    }


    void DisplayWS_ePaper_v2::showImage(Image &image, Point p1, Point p2, Rotation rotation) {

        fprintf(stderr, "ePaper showImage(%d,%d)\n", config.width, config.height);

        screenLock.lock();
        openSPI();
        digitalWrite(config.DC, 1);
        digitalWrite(config.CS, 0);

        int width = config.width + config.xOffset;
        int height = config.height + config.yOffset;

       
        writeCommand(0x10); // black/white

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

        enableDisplay();
        digitalWrite(config.CS, 1);
        screenLock.unlock();
    }






    void DisplayWS_ePaper_v2::setPartReg() {
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



    void DisplayWS_ePaper_v2::EPD_SetFullReg() {
        writeCommand(0X50);			//VCOM AND DATA INTERVAL SETTING
        writeData(0xb7);		    //WBmode:VBDF 17|D7 VBDW 97 VBDB 57		WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

        unsigned int count;
        writeCommand(0x20);
        for(count=0; count<44; count++) {
            writeData(EPD_2IN9D_lut_vcomDC[count]);
        }

        writeCommand(0x21);
        for(count=0; count<42; count++) {
            writeData(EPD_2IN9D_lut_ww[count]);
        }

        writeCommand(0x22);
        for(count=0; count<42; count++) {
            writeData(EPD_2IN9D_lut_bw[count]);
        }

        writeCommand(0x23);
        for(count=0; count<42; count++) {
            writeData(EPD_2IN9D_lut_wb[count]);
        }

        writeCommand(0x24);
        for(count=0; count<42; count++) {
            writeData(EPD_2IN9D_lut_bb[count]);
        }
    }

}