#pragma once
#include <iostream>
#include <WinSock2.h>
#include <string>
#include <fstream>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

enum Commands {
    GET_ = 1,
    LIST_ = 2,
    PUT_ = 3,
    DELETE_ = 4,
    INFO_ = 5,
    EXIT_ = 6
};

class CommonCode {
public:
    CommonCode(SOCKET clientSocket, const string& connectionPath, int clientNumber);

    void sendChunkedData(const char* data, int dataSize, int chunkSize) const;
    string receiveChunkedData() const;
    void sendFile(bool server, const string& clientFolder) const;
    void insertFile(bool server, const string& clientFolder) const;

private:
    SOCKET s;
    const string& connectionPath;
    int clientNumber;
};