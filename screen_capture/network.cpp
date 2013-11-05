#include "network.h"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

SOCKET connect(string hostname, int port) {
    cout << "Connecting..." << endl;

    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return 0;
    }

    SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // DWORD socket_timeout = 5000;
    // setsockopt(Socket, SOL_SOCKET, SO_SNDTIMEO, (const char*)socket_timeout, sizeof(socket_timeout));

    struct hostent *host;
    host = gethostbyname(hostname.c_str());

    SOCKADDR_IN SockAddr;
    SockAddr.sin_port = htons(port);
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

    if(connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0){
        return 0;
    }

    return Socket;
}

//------------------------------------------------------------------------------

void disconnect(SOCKET Socket) {
    closesocket(Socket);
    WSACleanup();
}

//------------------------------------------------------------------------------

bool send_image(SOCKET Socket, string encoded, string unique_id, string hostname) {
    time_t timestamp = time(NULL);
    ostringstream ostr_timestamp;
    ostr_timestamp << timestamp;
    string str_timestamp = ostr_timestamp.str();

    cout << "Time: " << timestamp << endl;

    string version = "1";

    ostringstream stream;

    stream << "POST / HTTP/1.1\r\n";
    stream << "Host: " << hostname << "\r\n";
    stream << "Accept: text/html,*/*\r\n";
    stream << "Connection: keep-alive\r\n";
    stream << "Content-Type: application/x-www-form-urlencoded\r\n";
    stream << "Content-Length: " << (15 + encoded.length()+1 + unique_id.length()+1 + str_timestamp.length()+1 + version.length()+1) << "\r\n\r\n";
    stream << "image=" << encoded << "&id=" << unique_id << "&time=" << str_timestamp << "&v=" << version;

    string request = stream.str();
    int result = send(Socket, request.c_str(), request.length(), 0);

    cout << "Send Image!" << endl;

    return (result > 0);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
