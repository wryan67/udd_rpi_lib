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
        writeByte(0x0f);//LUT from OTP?128x296
        writeByte(0x89);//Temperature sensor, boost and other related timing settings

        writeCommand(0x61);//resolution setting
        writeByte(0x80);
        writeByte(0x01);
        writeByte(0x28);

        writeCommand(0X50);//VCOM AND DATA INTERVAL SETTING			
        writeByte(0x77);//WBmode:VBDF 17|D7 VBDW 97 VBDB 57

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

         int cmd=0x12;  // clear screen;

        if (color.equals(RED)) {
            cmd = 0x13;
        }
        else if (color.equals(BLACK)) {
            cmd = 0x10;
        }

        writeCommand(cmd);

        if (cmd != 0x12) {
            for (int y = 0; y < config.height; ++y) {
                for (int x = 0; x < config.width; ++x) {
                    writeByte(0xFF);
                }
            }
            writeCommand(0x12);
        }

        
        
        pause();
        screenLock.unlock();
    }
}