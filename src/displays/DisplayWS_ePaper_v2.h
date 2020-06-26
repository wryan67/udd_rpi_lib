#pragma once
#include "Display.h"

// WS_ePaper_v2

namespace udd {

    class DisplayWS_ePaper_v2 : public Display {
    public:
        DisplayWS_ePaper_v2();

        void init() override;
        void readBusy();
    };
}
