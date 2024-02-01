#pragma once
#include "common.h"
#include <dirent.h>
#include <sys/stat.h>
#include <thread>
#include <filesystem>
namespace fs = filesystem;

class Server {
public:
    void runServer() {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = INADDR_ANY;
        bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
        listen(serverSocket, SOMAXCONN);
        cout << "Server is waiting for incoming connections on port " << port << "..." << endl;
        while (true) {
            SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
            cNumber++;
            if (cNumber == 3) {
                cout << "Server is full" << endl;
                break;
            }
            thread clientThread(&Server::handleClient, this, clientSocket, cNumber);
            clientThread.detach();
        }
        closesocket(serverSocket);
        WSACleanup();
    }

    void handleClient(SOCKET clientSocket, int clientNumber) {
        cout << "Client" << clientNumber << " is connected" << endl;
        string connectionPath = connectionFolder + "/Connection" + to_string(clientNumber);
        fs::create_directory(connectionPath);
        fs::create_directory(connectionPath + "/Server");
        fs::create_directory(connectionPath + "/Client");
        fs::copy(serverFolder + "/fileOne.txt", connectionPath + "/Server/fileOne.txt", fs::copy_options::overwrite_existing);
        fs::copy(serverFolder + "/fileTwo.txt", connectionPath + "/Server/fileTwo.txt", fs::copy_options::overwrite_existing);
        fs::copy(clientFolder + "/fileThree.txt", connectionPath + "/Client/fileThree.txt", fs::copy_options::overwrite_existing);
        processCommands(clientSocket, clientNumber, connectionPath);
        fs::remove_all(connectionPath);
        closesocket(clientSocket);
    }

     static void processCommands(SOCKET clientSocket, int clientNumber, const string& connectionPath) {
        send(clientSocket, reinterpret_cast<const char*>(&clientNumber), sizeof(int), 0);
        CommonCode commonCode(clientSocket, connectionPath, clientNumber);
        while (true) {
            int command;
            recv(clientSocket, reinterpret_cast<char*>(&command), sizeof(int), 0);
            if (command == GET_) {
                commonCode.sendFile(true);
            } else if (command == LIST_) {
                listCommand(commonCode, clientNumber);
            } else if (command == PUT_) {
                putCommand(commonCode);
            } else if (command == DELETE_) {
                deleteCommand(commonCode, clientNumber);
            } else if (command == INFO_) {
                infoCommand(commonCode, clientNumber);
            } else if (command == EXIT_) {
                cout << "Client" << clientNumber << " has disconnected" << endl;
                return;
            } else {
                cout << "Invalid command." << endl;
            }
        }
    }

    static void listCommand(CommonCode& commonCode, int clientNumber) {
        string path = commonCode.receiveChunkedData();
        string data;
        DIR* directory;
        struct dirent* entry;
        struct stat fileInfo;
        directory = opendir(path.c_str());
        if (directory == nullptr) {
            data = "Directory not found";
            commonCode.sendChunkedData(data.c_str(), data.size(), 10);
            return;
        }
        while ((entry = readdir(directory)) != nullptr) {
            string fullPath = path + "/" + entry->d_name;
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0 || stat(fullPath.c_str(), &fileInfo) == 0 && S_ISDIR(fileInfo.st_mode)) continue;
            data += entry->d_name;
            data += "\n";
        }
        closedir(directory);
        commonCode.sendChunkedData(data.c_str(), data.size(), 1024);
        cout << "Client" << clientNumber << ": List sent successfully" << endl;
    }

    static void putCommand(CommonCode& commonCode) {
        string filename = commonCode.receiveChunkedData();
        commonCode.sendChunkedData(filename.c_str(), filename.size(), 10);
        commonCode.insertFile(true);
    }

    static void deleteCommand(CommonCode& commonCode, int clientNumber) {
        string filename = commonCode.receiveChunkedData();
        if (remove(filename.c_str()) != 0) {
            commonCode.sendChunkedData("File not found", strlen("File not found"), 10);
            return;
        }
        commonCode.sendChunkedData("File deleted successfully", strlen("File deleted successfully"), 10);
        cout << "Client" << clientNumber << ": File deleted successfully" << endl;
    }

    static void infoCommand(CommonCode&  commonCode, int clientNumber) {
        string filename = commonCode.receiveChunkedData();
        struct stat fileInfo;
        if(stat(filename.c_str(), &fileInfo) != 0) {
            commonCode.sendChunkedData("Not found", strlen("Not found"), 10);
            return;
        }
        size_t pos = filename.find_last_of("/\\");
        string info = "File: " + filename.substr(pos + 1) + "\n";
        info += "Size: " + to_string(fileInfo.st_size) + " bytes\n";
        info += "Last modified: " + string(ctime(&fileInfo.st_mtime));
        commonCode.sendChunkedData(info.c_str(), info.size(), 1024);
        cout << "Client" << clientNumber << ": Info sent successfully" << endl;
    }

private:
    int port = 12345;
    int cNumber = 0;
    string serverFolder = "C:/Users/Admin/CLionProjects/Client-Server/Server PC";
    string clientFolder = "C:/Users/Admin/CLionProjects/Client-Server/Client PC";
    string connectionFolder = "C:/Users/Admin/CLionProjects/ClientServerDb";
};
