#include <iostream>
#include <string>
#include <windows.h>
#include <unistd.h>

using namespace std;

#include "gdi/include/GdiPlus.h"

using namespace Gdiplus;

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
   UINT  num = 0;          // number of image encoders
   UINT  size = 0;         // size of the image encoder array in bytes

   ImageCodecInfo* pImageCodecInfo = NULL;

   GetImageEncodersSize(&num, &size);
   if(size == 0)
      return -1;  // Failure

   pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
   if(pImageCodecInfo == NULL)
      return -1;  // Failure

   GetImageEncoders(num, size, pImageCodecInfo);

   for(UINT j = 0; j < num; ++j)
   {
      if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
      {
         *pClsid = pImageCodecInfo[j].Clsid;
         free(pImageCodecInfo);
         return j;  // Success
      }
   }

   free(pImageCodecInfo);
   return -1;  // Failure
}

void screen_capture() {
    unsigned int cx = GetSystemMetrics(SM_CXSCREEN), cy = GetSystemMetrics(SM_CYSCREEN);
    HDC hScreenDC = ::GetDC(NULL);
    HDC hMemDC = CreateCompatibleDC(hScreenDC);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, cx, cy);
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
    BitBlt(hMemDC, 0, 0, cx, cy, hScreenDC, 0, 0, SRCCOPY);
    SelectObject(hMemDC, hOldBitmap);
    ::ReleaseDC(NULL, hScreenDC);

    size_t headerSize = sizeof(BITMAPINFOHEADER)+3*sizeof(RGBQUAD);
    BYTE* pHeader = new BYTE[headerSize];
    LPBITMAPINFO pbmi = (LPBITMAPINFO)pHeader;
    memset(pHeader, 0, headerSize);
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biBitCount = 0;
    if (!GetDIBits(hMemDC, hBitmap, 0, 0, NULL, pbmi, DIB_RGB_COLORS))
        return;

    BITMAPFILEHEADER bmf;
    if (pbmi->bmiHeader.biSizeImage <= 0)
        pbmi->bmiHeader.biSizeImage=pbmi->bmiHeader.biWidth*abs(pbmi->bmiHeader.biHeight)*(pbmi->bmiHeader.biBitCount+7)/8;
    BYTE* pData = new BYTE[pbmi->bmiHeader.biSizeImage];
    bmf.bfType = 0x4D42; bmf.bfReserved1 = bmf.bfReserved2 = 0;
    bmf.bfSize = sizeof(BITMAPFILEHEADER)+ headerSize + pbmi->bmiHeader.biSizeImage;
    bmf.bfOffBits = sizeof(BITMAPFILEHEADER) + headerSize;
    if (!GetDIBits(hMemDC, hBitmap, 0, abs(pbmi->bmiHeader.biHeight), pData, pbmi, DIB_RGB_COLORS))
    {
        delete pData;
        return;
    }

    FILE* hFile = fopen("_.bmp", "wb");
    fwrite(&bmf, sizeof(BITMAPFILEHEADER), 1, hFile);
    fwrite(pbmi, headerSize, 1, hFile);
    fwrite(pData, pbmi->bmiHeader.biSizeImage, 1, hFile);
    fclose(hFile);

    DeleteObject(hBitmap);
    DeleteDC(hMemDC);

    delete [] pData;
}

void convert_to_jpeg(int counter) {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    Image image(L"_.bmp");

    CLSID jpgClsid;
    GetEncoderClsid(L"image/jpeg", &jpgClsid);

    char buffer[10];
    sprintf(buffer, "_%d.jpg", counter);
    string str_buffer = buffer;
    wstring image_name(str_buffer.begin(), str_buffer.end());
    image.Save(image_name.c_str(), &jpgClsid, NULL);
}

// To compile
// i586-mingw32msvc-g++ -o screen_capture.exe screen_capture.cpp -L./gdi/lib -lgdi32 -lgdiplus

int main() {
    int c = 0;

    while (true) {
      screen_capture();
      convert_to_jpeg(c);

      c++;
      if (c >= 10) c = 0;

      usleep(1000 * 100);
    }

    return 0;
}
