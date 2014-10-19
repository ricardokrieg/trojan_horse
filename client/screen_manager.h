#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>
#include <memory>

using namespace std;

#include "gdi/include/GdiPlus.h"

using namespace Gdiplus;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

class ScreenManager {
    public:
        ScreenManager(void);
        ~ScreenManager(void);

        string capture(void);

        CLSID jpgClsid;
        HWND hDesktopWnd;
        HDC hDesktopDC;
        HDC hCaptureDC;
};

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
