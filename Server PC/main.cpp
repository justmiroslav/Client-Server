#include <iostream>
#include <WinSock2.h>
#include <string>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
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

void getCommand(SOCKET clientSocket) {
    string filename = receiveChunkedData(clientSocket);
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

void listCommand(SOCKET clientSocket) {
    string path = receiveChunkedData(clientSocket);
    string data;
    DIR* directory;
    struct dirent* entry;
    struct stat fileInfo;
    directory = opendir(path.c_str());
    if (directory == nullptr) {
        data = "Directory not found";
        sendChunkedData(clientSocket, data.c_str(), data.size(), 10);
        return;
    }
    while ((entry = readdir(directory)) != nullptr) {
        string fullPath = path + "/" + entry->d_name;
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || stat(fullPath.c_str(), &fileInfo) == 0 && S_ISDIR(fileInfo.st_mode)) continue;
        data += entry->d_name;
        data += "\n";
    }
    closedir(directory);
    sendChunkedData(clientSocket, data.c_str(), data.size(), 1024);
    cout << "List sent successfully" << endl;
}

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
    while (true) {
        int command;
        int recvResult = recv(clientSocket, reinterpret_cast<char*>(&command), sizeof(int), 0);
        if (recvResult == SOCKET_ERROR || recvResult == 0) {
            cout << "Client disconnected." << endl;
            break;
        }
        if (command == GET_) {
            getCommand(clientSocket);
        } else if (command == LIST_) {
            listCommand(clientSocket);
        } else {
            cout << "Invalid command." << endl;
        }
    }
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}