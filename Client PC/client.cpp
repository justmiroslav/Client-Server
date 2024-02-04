#include "client.h"

void Client::runClient() {
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

void Client::processCommands() {
    int clientNumber;
    recv(clientSocket, reinterpret_cast<char*>(&clientNumber), sizeof(int), 0);
    string connectionPath = connectionFolder + "/Client" + to_string(clientNumber);
    CommonCode commonCode(clientSocket, connectionPath, clientNumber);
    commonCode.sendChunkedData(clientFolder.c_str(), clientFolder.size(), 10);
    while (true) {
        int command;
        cout << "Enter command (GET->1;LIST->2;PUT->3;DELETE->4;INFO->5;EXIT->6): " << endl;
        cin >> command;
        send(clientSocket, reinterpret_cast<const char*>(&command), sizeof(int), 0);
        if (command == GET_) {
            sendPath(commonCode);
            commonCode.insertFile(false, clientFolder);
        } else if (command == LIST_) {
            sendPath(commonCode);
            getResponse(commonCode);
        } else if (command == PUT_) {
            sendPath(commonCode);
            commonCode.sendFile(false, clientFolder);
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

void Client::sendPath(CommonCode& commonCode) {
    string filename;
    cout << "Enter filename/dirname: " << endl;
    cin.ignore();
    getline(cin, filename);
    commonCode.sendChunkedData(filename.c_str(), filename.size(), 10);
}

void Client::getResponse(CommonCode& commonCode) {
    string response = commonCode.receiveChunkedData();
    cout << response << endl;
}