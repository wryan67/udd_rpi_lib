#include "DisplayWS_ePaper_v2.h"
#include <wiringPi.h>

namespace udd {

    DisplayWS_ePaper_v2::DisplayWS_ePaper_v2() : Display() {}

    void DisplayWS_ePaper_v2::init() {
        reset();

        writeCommand(0x04);
        readBusy();

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
        unsigned char busy;
        do {
            writeCommand(0x71);
            busy = digitalRead(config.busyPin);
            busy = !(busy & 0x01);
        } while (busy);
        delay(200);
    }

}