#include "network.h"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

SOCKET connect(string hostname, int port) {
    WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return 0;
    }

    SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

void send_image(SOCKET Socket, string encoded, string hostname) {
    time_t timestamp = time(NULL);
    ostringstream ostr_timestamp;
    ostr_timestamp << timestamp;
    string str_timestamp = ostr_timestamp.str();

    ostringstream stream;

    stream << "POST / HTTP/1.1\r\n";
    stream << "Host: " << hostname << "\r\n";
    stream << "Accept: text/html,*/*\r\n";
    stream << "Connection: keep-alive\r\n";
    stream << "Content-Type: application/x-www-form-urlencoded\r\n";
    stream << "Content-Length: " << (1+encoded.length()+10+str_timestamp.length()+1) << "\r\n\r\n";
    stream << "image=" << encoded << "&time=" << timestamp;

    string request = stream.str();
    send(Socket, request.c_str(), request.length(), 0);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------