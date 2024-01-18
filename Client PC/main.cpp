#include <iostream>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <string>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

enum Commands {
    GET_ = 1,
    LIST_ = 2
};

void sendChunkedData(SOCKET clientSocket, const char* data, int dataSize, int chunkSize) {
    int totalSent = 0;
    send(clientSocket, reinterpret_cast<const char*>(&dataSize), sizeof(int), 0);
    while (totalSent < dataSize) {
        int remaining = dataSize - totalSent;
        int currentChunkSize = (remaining < chunkSize) ? remaining : chunkSize;
        send(clientSocket, reinterpret_cast<const char*>(&currentChunkSize), sizeof(int), 0);
        send(clientSocket, data + totalSent, currentChunkSize, 0);
        totalSent += currentChunkSize;
    }
}

string receiveChunkedData(SOCKET clientSocket) {
    int chunkSize, totalSize = 0;
    string data;
    recv(clientSocket, reinterpret_cast<char*>(&totalSize), sizeof(int), 0);
    do {
        recv(clientSocket, reinterpret_cast<char*>(&chunkSize), sizeof(int), 0);
        if (chunkSize > 0) {
            char* buffer = new char[chunkSize + 1];
            recv(clientSocket, buffer, chunkSize, 0);
            data.append(buffer, chunkSize);
            delete[] buffer;
        }
    } while (data.size() < totalSize);
    return data;
}

void sendPath(SOCKET clientSocket) {
    string filename;
    cout << "Enter filename/dirname: " << endl;
    cin.ignore();
    getline(cin, filename);
    sendChunkedData(clientSocket, filename.c_str(), filename.size(), 10);
}

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
    while (true) {
        int command;
        cout << "Enter command: " << endl;
        cin >> command;
        send(clientSocket, reinterpret_cast<const char*>(&command), sizeof(int), 0);
        if (command == GET_) {
            sendPath(clientSocket);
            string data = receiveChunkedData(clientSocket);
            cout << data << endl;
        } else if (command == LIST_) {
            sendPath(clientSocket);
            string data = receiveChunkedData(clientSocket);
            cout << data << endl;
        } else {
            cout << "Invalid command." << endl;
            break;
        }
    }
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}