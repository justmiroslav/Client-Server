#pragma once
#include "C:\Users\Admin\CLionProjects\Client-Server\Server PC\common.h"
#include <Ws2tcpip.h>

void sendPath(SOCKET clientSocket) {
    string filename;
    cout << "Enter filename/dirname: " << endl;
    cin.ignore();
    getline(cin, filename);
    sendChunkedData(clientSocket, filename.c_str(), filename.size(), 10);
}

void getResponse(SOCKET clientSocket) {
    string data = receiveChunkedData(clientSocket);
    cout << data << endl;
}

void commandProcessing(SOCKET clientSocket) {
    while (true) {
        int command;
        cout << "Enter command (GET->1;LIST->2;PUT->3;DELETE->4;INFO->5;EXIT->6): " << endl;
        cin >> command;
        send(clientSocket, reinterpret_cast<const char*>(&command), sizeof(int), 0);
        if (command == GET_) {
            sendPath(clientSocket);
            sendFile(clientSocket);
        } else if (command == LIST_) {
            sendPath(clientSocket);
            getResponse(clientSocket);
        } else if (command == PUT_) {
            sendPath(clientSocket);
            insertFile(clientSocket, "C:/Users/Admin/CLionProjects/Client-Server/Client PC");
        } else if (command == DELETE_) {
            sendPath(clientSocket);
            getResponse(clientSocket);
        } else if (command == INFO_) {
            sendPath(clientSocket);
            getResponse(clientSocket);
        } else if (command == EXIT_) {
            cout << "Program finished" << endl;
        } else {
            cout << "Invalid command." << endl;
            return;
        }
    }
}
