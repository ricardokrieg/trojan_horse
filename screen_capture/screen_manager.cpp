#include "helper.h"
#include "screen_manager.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

ScreenManager::ScreenManager(void) {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    GetEncoderClsid(L"image/jpeg", &(this->jpgClsid));

    this->hDesktopWnd = GetDesktopWindow();
    this->hDesktopDC = GetDC(this->hDesktopWnd);
    this->hCaptureDC = CreateCompatibleDC(this->hDesktopDC);
}

//------------------------------------------------------------------------------

ScreenManager::~ScreenManager(void) {
    ReleaseDC(this->hDesktopWnd, this->hDesktopDC);
    DeleteDC(this->hCaptureDC);
}

//------------------------------------------------------------------------------

string ScreenManager::capture(void) {
    cout << "Screenshot!" << endl;

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HBITMAP hCaptureBitmap = CreateCompatibleBitmap(this->hDesktopDC, width, height);

    SelectObject(this->hCaptureDC, hCaptureBitmap);
    BitBlt(this->hCaptureDC, 0, 0, width, height, this->hDesktopDC, 0, 0, SRCCOPY|CAPTUREBLT);

    Bitmap bitmap(hCaptureBitmap, (HPALETTE)1);

    DeleteObject(hCaptureBitmap);

    IStream* stream = NULL;

    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, 0);
    CreateStreamOnHGlobal(hGlobal, TRUE, &stream);

    bitmap.Save(stream, &(this->jpgClsid), NULL);

    LARGE_INTEGER i = {0};
    stream->Seek(i, (DWORD)0, NULL);

    string buffer;
    BYTE c_buffer;
    ULONG read_bytes;

    while (true) {
        HRESULT result = stream->Read(&c_buffer, 1, &read_bytes);

        buffer += c_buffer;

        if (result == S_OK) {
            if (read_bytes < 1) {
                break;
            }
        } else {
            break;
        }
    }

    stream->Release();

    string encoded = base64_encode(reinterpret_cast<const unsigned char*>(buffer.c_str()), buffer.length());

    return encoded;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
