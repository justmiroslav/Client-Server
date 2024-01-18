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

void sendFile(SOCKET clientSocket) {
    string filename = receiveChunkedData(clientSocket);
    sendChunkedData(clientSocket, filename.c_str(), filename.size(), 10);
    ifstream file(filename);
    if (!file.is_open()) {
        sendChunkedData(clientSocket, "File not found", strlen("File not found"), 10);
    }
    string data;
    char character;
    while (file.get(character)) {
        data += character;
    }
    file.close();
    sendChunkedData(clientSocket, data.c_str(), data.size(), 1024);
    cout << "File sent successfully" << endl;
}

void insertFile(SOCKET clientSocket, const string& rootFolder) {
    string filename = receiveChunkedData(clientSocket);
    string data = receiveChunkedData(clientSocket);
    size_t pos = filename.find_last_of("/\\");
    string fullPath = rootFolder + "/" + filename.substr(pos + 1);
    ofstream file(fullPath.c_str());
    if (!file.is_open()) {
        sendChunkedData(clientSocket, "File not found", strlen("File not found"), 10);
    }
    file << data;
    file.close();
    cout << "File inserted successfully" << endl;
}