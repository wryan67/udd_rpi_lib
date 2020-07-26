#pragma once
#include "Display.h"

namespace udd {

    class DisplayST7789R : public Display {
    public:
        DisplayST7789R();

        void init() override;

        void setWindow(int x1, int y1, int x2, int y2, Rotation rotation);

    };
}
