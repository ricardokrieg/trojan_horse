#include "network.h"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

SOCKET connect(string hostname, int port) {
    cout << "C." << endl;

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

bool send_image(SOCKET Socket, string encoded, string unique_id, string hostname) {
    time_t timestamp = time(NULL);
    ostringstream ostr_timestamp;
    ostr_timestamp << timestamp;
    string str_timestamp = ostr_timestamp.str();

    cout << "T " << timestamp << endl;

    string version = "1";

    ostringstream stream;

    stream << "image=" << encoded << "&id=" << unique_id << "&time=" << str_timestamp << "&v=" << version << "&\n";

    string request = stream.str();
    int result = send(Socket, request.c_str(), request.length(), 0);

    cout << "I" << endl;

    return (result > 0);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
