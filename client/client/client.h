
#ifndef CLIENT_H
#define CLIENT_H
#include <winsock2.h>
#include <ws2tcpip.h>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include "fileService.h"
#include "controller.h"
#include <csignal>
using namespace std;
#pragma comment(lib, "ws2_32.lib")


class Client {
private:
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;
    static void signalHandler(int signum);
    static Client* instance;

public:
    Client();
    ~Client();
    void run();
    void closeConnection();
};
#endif // CLIENT_H