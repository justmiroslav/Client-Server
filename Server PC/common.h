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
    CommonCode(SOCKET clientSocket, const string& connectionPath, int clientNumber) : clientSocket(clientSocket), connectionPath(connectionPath), clientNumber(clientNumber) {}

    void sendChunkedData(const char* data, int dataSize, int chunkSize) const {
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

    string receiveChunkedData() const {
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

    void sendFile(bool server) const {
        string filename = receiveChunkedData();
        sendChunkedData(filename.c_str(), filename.size(), 10);
        ifstream file(server ? connectionPath + "/Server/" + filename : connectionPath + "/Client/" + filename);
        if (!file.is_open()) {
            sendChunkedData("File not found", strlen("File not found"), 10);
            return;
        }
        string data;
        char character;
        while (file.get(character)) {
            data += character;
        }
        file.close();
        sendChunkedData(data.c_str(), data.size(), 1024);
        cout << (server ? "Server" : "Client" + to_string(clientNumber)) << ": File sent successfully" << endl;
    }

    void insertFile(bool server) const {
        string filename = receiveChunkedData();
        string data = receiveChunkedData();
        if (data == "File not found") {
            cout << "Invalid data" << endl;
            return;
        }
        ofstream file(server ? connectionPath + "/Server/" + filename : connectionPath + "/Client/" + filename);
        file << data;
        file.close();
        cout << (server ? "Server" : "Client" + to_string(clientNumber)) << ": File received successfully" << endl;
    }

private:
    SOCKET clientSocket;
    const string& connectionPath;
    int clientNumber;
};