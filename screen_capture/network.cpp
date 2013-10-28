#include "network.h"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

SOCKET connect(string hostname, int port) {
    WSADATA wsaData;

    cout << "Starting WSA ..." << endl;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return 0;
    }

    cout << "Creating Socket ..." << endl;
    SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct hostent *host;
    host = gethostbyname(hostname.c_str());

    cout << "Preparing Host ..." << endl;
    SOCKADDR_IN SockAddr;
    SockAddr.sin_port = htons(port);
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);

    cout << "Connecting ..." << endl;
    if(connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0){
        return 0;
    }

    cout << "Connected ..." << endl;
    return Socket;
}

//------------------------------------------------------------------------------

void disconnect(SOCKET Socket) {
    cout << "Disconnecting ..." << endl;
    closesocket(Socket);
    WSACleanup();
}

//------------------------------------------------------------------------------

void send_image(SOCKET Socket, string encoded, string hostname) {
    cout << "Preparing Timestamp ..." << endl;
    time_t timestamp = time(NULL);
    ostringstream ostr_timestamp;
    ostr_timestamp << timestamp;
    string str_timestamp = ostr_timestamp.str();

    ostringstream stream;

    cout << "Preparing POST ..." << endl;
    stream << "POST / HTTP/1.1\r\n";
    stream << "Host: " << hostname << "\r\n";
    stream << "Accept: text/html,*/*\r\n";
    stream << "Content-Type: application/x-www-form-urlencoded\r\n";
    stream << "Content-Length: " << (1+encoded.length()+10+str_timestamp.length()+1) << "\r\n\r\n";
    stream << "image=" << encoded << "&time=" << timestamp;

    cout << "Sending ..." << endl;
    string request = stream.str();
    cout << send(Socket, request.c_str(), request.length(), 0) << endl;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
