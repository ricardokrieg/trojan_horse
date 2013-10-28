#include "helper.h"
#include "screen_manager.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

ScreenManager::ScreenManager(void) {
    cout << "Initializing ScreenManager ..." << endl;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    GetEncoderClsid(L"image/jpeg", &(this->jpgClsid));

    cout << "Getting Attributes ..." << endl;
    this->hDesktopWnd = GetDesktopWindow();
    this->hDesktopDC = GetDC(this->hDesktopWnd);
    this->hCaptureDC = CreateCompatibleDC(this->hDesktopDC);
}

//------------------------------------------------------------------------------

ScreenManager::~ScreenManager(void) {
    cout << "Destroying ScreenManager ..." << endl;
    ReleaseDC(this->hDesktopWnd, this->hDesktopDC);
    DeleteDC(this->hCaptureDC);
}

//------------------------------------------------------------------------------

string ScreenManager::capture(void) {
    cout << "Getting Screen Metrics ..." << endl;
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    cout << "Creating Bitmap ..." << endl;
    HBITMAP hCaptureBitmap = CreateCompatibleBitmap(this->hDesktopDC, width, height);

    cout << "Bliting ..." << endl;
    SelectObject(this->hCaptureDC, hCaptureBitmap);
    BitBlt(this->hCaptureDC, 0, 0, width, height, this->hDesktopDC, 0, 0, SRCCOPY|CAPTUREBLT);

    cout << "Capturing Bitmap ..." << endl;
    Bitmap bitmap(hCaptureBitmap, (HPALETTE)1);

    DeleteObject(hCaptureBitmap);

    cout << "Saving Bitmap ..." << endl;
    bitmap.Save(L"image.jpg", &(this->jpgClsid), NULL);

    const string to_encode = file_to_string("image.jpg");
    cout << "Encoding ..." << endl;
    string encoded = base64_encode(reinterpret_cast<const unsigned char*>(to_encode.c_str()), to_encode.length());

    cout << "Done!" << endl;
    return encoded;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
