#pragma once
#include "common.h"
#include <dirent.h>
#include <sys/stat.h>
#include <thread>
#include <filesystem>
namespace fs = filesystem;

class Server {
public:
    void runServer();
    void handleClient(SOCKET clientSocket, int clientNumber);
    void processCommands(SOCKET clientSocket, int clientNumber, const string& connectionPath);
    void listCommand(CommonCode& commonCode, int clientNumber);
    void putCommand(CommonCode& commonCode, const string& clientFolder);
    void deleteCommand(CommonCode& commonCode, int clientNumber);
    void infoCommand(CommonCode&  commonCode, int clientNumber);

private:
    int port = 12345;
    int cNumber = 0;
    string serverFolder = "C:/Users/Admin/CLionProjects/Client-Server/Server PC";
    string connectionFolder = "C:/Users/Admin/CLionProjects/ClientServerDb";
};