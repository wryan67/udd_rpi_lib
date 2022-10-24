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
        void clearScreen(const Color &color) override;
        void showImage(const Image& image) override;
        void showImage(const Image& image, Rotation rotation) override;
        void showImage(const Image& image, Point p1, Point p2, Rotation rotation) override;
        void EPD_SetFullReg();
        void setPartReg();
        void enableDisplay();
    };
}
