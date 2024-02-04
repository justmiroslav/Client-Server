#pragma once
#include "common.h"
#include <Ws2tcpip.h>

class Client {
public:
    void runClient();
    void processCommands();
    static void sendPath(CommonCode& commonCode);
    static void getResponse(CommonCode& commonCode);

private:
    SOCKET clientSocket;
    const char* serverIp = "127.0.0.1";
    int port = 12345;
    string connectionFolder = "C:/Users/Admin/CLionProjects/ClientServerDb";
    string clientFolder = "C:/Users/Admin/CLionProjects/Client-Server/Client PC";
};