#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
// Linking the library needed for network communication
#pragma comment(lib, "ws2_32.lib")
using namespace std;

enum Commands {
    GET_ = 1,
    LIST_ = 2,
    PUT_ = 3,
    DELETE_ = 4,
    INFO_ = 5
};

void sendChunkedData(SOCKET clientSocket, const char* data, int dataSize, int chunkSize) {
    send(clientSocket, reinterpret_cast<const char*>(&dataSize), sizeof(int), 0);
    int totalSent = 0;
    while (totalSent < dataSize) {
        int remaining = dataSize - totalSent;
        int currentChunkSize = (remaining < chunkSize) ? remaining : chunkSize;
        send(clientSocket, data + totalSent, currentChunkSize, 0);
        totalSent += currentChunkSize;
        cout << "Sent chunk of size " << currentChunkSize << endl;
    }
}

int main()
{
    // Initialize Winsock
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    // Client configuration
    int port = 12345;
    const char* serverIp = "127.0.0.1";
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(serverIp);
    // Connect to the server
    connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    // Communication with the server
    while (true) {
        int command;
        char buffer[1024];
        memset(buffer, 0, 1024);
        cout << "Enter the command: " << endl;
        cin >> command;
        if (command == GET_) {
            const char* data = "This is a sample data to be sent in chunks. ";
            int chunkSize = 10;
            sendChunkedData(clientSocket, data, strlen(data), chunkSize);
            cout << "All chunks sent to the server." << endl;
        } else {
            cout << "Unexpected command" << endl;
            break;
        }
    }
    // Clean up
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}