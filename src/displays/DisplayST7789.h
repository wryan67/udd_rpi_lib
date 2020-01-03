#pragma once
#include <Display.h>

namespace udd {

    class DisplayST7789 : public Display {
    public:
        DisplayST7789();
        DisplayST7789(DisplayConfigruation config);

    protected:
        void init();

    };
}
