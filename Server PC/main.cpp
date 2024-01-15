#include <iostream>
#include <WinSock2.h>
// Linking the library needed for network communication
#pragma comment(lib, "ws2_32.lib")
using namespace std;

void receiveChunkedData(SOCKET clientSocket) {
    int totalSize;
    recv(clientSocket, reinterpret_cast<char*>(&totalSize), sizeof(int), 0);
    cout << "Total size received: " << totalSize << endl;
    char* assembledData = new char[totalSize + 1 /*End of string*/];
    int totalReceived = 0;
    while (totalReceived < totalSize) {
        char buffer[1024];
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        memcpy(assembledData + totalReceived, buffer, bytesReceived);
        totalReceived += bytesReceived;
        cout << "Received chunk of size " << bytesReceived << endl;
    }
    assembledData[totalReceived] = '\0';
    cout << "Assembled data on the server: " << assembledData << endl;
    delete[] assembledData;
}

int main()
{
    // Initialize Winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    // Server configuration
    int port = 12345;
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    // Bind the socket
    bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    // Listen for incoming connections
    listen(serverSocket, SOMAXCONN);
    cout << "Server is waiting for incoming connections on port " << port << "..." << endl;
    // Accept a client connection
    SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
    cout << "Client connected." << endl;
    // Communication with the client
    receiveChunkedData(clientSocket);
    // Clean up
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}