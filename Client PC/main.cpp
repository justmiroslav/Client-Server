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
        cout << "Enter command (GET->1;LIST->2;PUT->3;DELETE->4;INFO->5;EXIT->6): " << endl;
        cin >> command;
        send(clientSocket, reinterpret_cast<const char*>(&command), sizeof(int), 0);
        if (command == GET_) {
            sendPath(clientSocket);
            insertFile(clientSocket, "C:/Users/Admin/CLionProjects/Client-Server/Client PC");
        } else if (command == LIST_) {
            sendPath(clientSocket);
            getResponse(clientSocket);
        } else if (command == PUT_) {
            sendPath(clientSocket);
            sendFile(clientSocket);
        } else if (command == DELETE_) {
            sendPath(clientSocket);
            getResponse(clientSocket);
        } else if (command == INFO_) {
            sendPath(clientSocket);
            getResponse(clientSocket);
        } else if (command == EXIT_) {
            cout << "Program finished" << endl;
            break;
        } else {
            cout << "Invalid command." << endl;
        }
    }
    closesocket(clientSocket);
    WSACleanup();
    return 0;
}