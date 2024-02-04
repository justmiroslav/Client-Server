#include "server.h"

void Server::runServer() {
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

void Server::handleClient(SOCKET clientSocket, int clientNumber) {
    cout << "Client" << clientNumber << " is connected" << endl;
    string connectionPath = connectionFolder + "/Client" + to_string(clientNumber);
    fs::create_directory(connectionPath);
    fs::copy(serverFolder + "/fileOne.txt", connectionPath + "/fileOne.txt", fs::copy_options::overwrite_existing);
    fs::copy(serverFolder + "/fileTwo.txt", connectionPath + "/fileTwo.txt", fs::copy_options::overwrite_existing);
    processCommands(clientSocket, clientNumber, connectionPath);
    fs::remove_all(connectionPath);
    closesocket(clientSocket);
}

void Server::processCommands(SOCKET clientSocket, int clientNumber, const string& connectionPath) {
    send(clientSocket, reinterpret_cast<const char*>(&clientNumber), sizeof(int), 0);
    CommonCode commonCode(clientSocket, connectionPath, clientNumber);
    string clientFolder = commonCode.receiveChunkedData();
    while (true) {
        int command;
        recv(clientSocket, reinterpret_cast<char*>(&command), sizeof(int), 0);
        if (command == GET_) {
            commonCode.sendFile(true, clientFolder);
        } else if (command == LIST_) {
            listCommand(commonCode, clientNumber);
        } else if (command == PUT_) {
            putCommand(commonCode, clientFolder);
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

void Server::listCommand(CommonCode& commonCode, int clientNumber) {
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

void Server::putCommand(CommonCode& commonCode, const string& clientFolder) {
    string filename = commonCode.receiveChunkedData();
    commonCode.sendChunkedData(filename.c_str(), filename.size(), 10);
    commonCode.insertFile(true, clientFolder);
}

void Server::deleteCommand(CommonCode& commonCode, int clientNumber) {
    string filename = commonCode.receiveChunkedData();
    if (remove(filename.c_str()) != 0) {
        commonCode.sendChunkedData("File not found", strlen("File not found"), 10);
        return;
    }
    commonCode.sendChunkedData("File deleted successfully", strlen("File deleted successfully"), 10);
    cout << "Client" << clientNumber << ": File deleted successfully" << endl;
}

void Server::infoCommand(CommonCode&  commonCode, int clientNumber) {
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