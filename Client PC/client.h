#pragma once
#include "C:\Users\Admin\CLionProjects\Client-Server\Server PC\common.h"
#include <Ws2tcpip.h>

class Client {
public:
    void runClient() {
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
        clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = inet_addr(serverIp);
        connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
        processCommands();
        closesocket(clientSocket);
        WSACleanup();
    }

    void processCommands() const {
        int clientNumber;
        recv(clientSocket, reinterpret_cast<char*>(&clientNumber), sizeof(int), 0);
        CommonCode commonCode(clientSocket, connectionFolder + "/Connection" + to_string(clientNumber) + "/Server", connectionFolder + "/Connection" + to_string(clientNumber) + "/Client");
        while (true) {
            int command;
            cout << "Enter command (GET->1;LIST->2;PUT->3;DELETE->4;INFO->5;EXIT->6): " << endl;
            cin >> command;
            send(clientSocket, reinterpret_cast<const char*>(&command), sizeof(int), 0);
            if (command == GET_) {
                sendPath(commonCode);
                commonCode.insertFile(false);
            } else if (command == LIST_) {
                sendPath(commonCode);
                getResponse(commonCode);
            } else if (command == PUT_) {
                sendPath(commonCode);
                commonCode.sendFile(false);
            } else if (command == DELETE_) {
                sendPath(commonCode);
                getResponse(commonCode);
            } else if (command == INFO_) {
                sendPath(commonCode);
                getResponse(commonCode);
            } else if (command == EXIT_) {
                cout << "Connection stopped..." << endl;
                return;
            } else {
                cout << "Invalid command." << endl;
            }
        }
    }

    static void sendPath(CommonCode& commonCode) {
        string filename;
        cout << "Enter filename/dirname: " << endl;
        cin.ignore();
        getline(cin, filename);
        commonCode.sendChunkedData(filename.c_str(), filename.size(), 10);
    }

    static void getResponse(CommonCode& commonCode) {
        string response = commonCode.receiveChunkedData();
        cout << response << endl;
    }

private:
    SOCKET clientSocket;
    const char* serverIp = "127.0.0.1";
    int port = 12345;
    string connectionFolder = "C:/Users/Admin/OneDrive/Робочий стіл/Client-Server";
};
