#include "common.h"
#include <dirent.h>
#include <sys/stat.h>

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

void putCommand(SOCKET clientSocket) {
    string filename = receiveChunkedData(clientSocket);
    sendChunkedData(clientSocket, filename.c_str(), filename.size(), 10);
    insertFile(clientSocket, "C:/Users/Admin/CLionProjects/Client-Server/Server PC");
}

void deleteCommand(SOCKET clientSocket) {
    string filename = receiveChunkedData(clientSocket);
    if (remove(filename.c_str()) != 0) {
        sendChunkedData(clientSocket, "File not found", strlen("File not found"), 10);
        return;
    }
    sendChunkedData(clientSocket, "File sent successfully", strlen("File sent successfully"), 10);
    cout << "File deleted successfully" << endl;
}

void infoCommand(SOCKET clientSocket) {
    string filename = receiveChunkedData(clientSocket);
    struct stat fileInfo;
    if(stat(filename.c_str(), &fileInfo) != 0) {
        sendChunkedData(clientSocket, "File not found", strlen("File not found"), 10);
        return;
    }
    size_t pos = filename.find_last_of("/\\");
    string info = "File: " + filename.substr(pos + 1) + "\n";
    info += "Size: " + to_string(fileInfo.st_size) + " bytes\n";
    info += "Last modified: " + string(ctime(&fileInfo.st_mtime));
    sendChunkedData(clientSocket, info.c_str(), info.size(), 1024);
    cout << "Info sent successfully" << endl;
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
        recv(clientSocket, reinterpret_cast<char*>(&command), sizeof(int), 0);
        if (command == GET_) {
            sendFile(clientSocket);
        } else if (command == LIST_) {
            listCommand(clientSocket);
        } else if (command == PUT_) {
            putCommand(clientSocket);
        } else if (command == DELETE_) {
            deleteCommand(clientSocket);
        } else if (command == INFO_) {
            infoCommand(clientSocket);
        } else if (command == EXIT_) {
            cout << "Program finished" << endl;
            break;
        } else {
            cout << "Invalid command." << endl;
        }
    }
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}