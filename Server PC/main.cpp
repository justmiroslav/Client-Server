#include "server.h"

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    int port = 12345;
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);
    cout << "Server is waiting for incoming connections on port " << port << "..." << endl;
    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    cout << "Client connected." << endl;
    commandProcessing(clientSocket);
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}