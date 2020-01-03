#pragma once
#include <Display.h>

namespace udd {

    class DisplayST7789R : public Display {
    public:
        DisplayST7789R();
        DisplayST7789R(DisplayConfigruation config);

    protected:
        void init();

    };
}
