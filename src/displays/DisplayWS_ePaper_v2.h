#pragma once
#include "Display.h"

// WS_ePaper_v2

namespace udd {

    class DisplayWS_ePaper_v2 : public Display {
    public:
        DisplayWS_ePaper_v2();

        void init() override;
        void reset() override;
        void readBusy() override;
        void clear(Color color) override;
        void showImage(Image image) override;
        void showImage(Image image, Rotation rotation) override;

    };
}
