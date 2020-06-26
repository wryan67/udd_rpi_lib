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
            for (int x = 0; x < config.width; ++x) {
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
            for (int x = 0; x < config.width; ++x) {
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
}