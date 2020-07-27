#pragma once
#include "Display.h"

namespace udd {

    class DisplayST7789R : public Display {
    public:
        DisplayST7789R();

        void init() override;


    };
}
