#include "client.h"

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    int port = 12345;
    const char* serverIp = "127.0.0.1";
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(serverIp);
    connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    commandProcessing(clientSocket);
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}