#pragma once
#include "Display.h"

// WS_ePaper_v2

namespace udd {

    class DisplayWS_ePaper_v2 : public Display {
    private:
    

    public:
        DisplayWS_ePaper_v2();

        void init() override;
        void initPartial();
        void reset() override;
        void readBusy() override;
        void clearScreen(Color color) override;
        void showImage(Image &image) override;
        void showImage(Image &image, Rotation rotation) override;
        void showImagePartial(Image image, Rotation rotation);
        void EPD_SetFullReg();
        void setPartReg();
        void enableDisplay();
    };
}
