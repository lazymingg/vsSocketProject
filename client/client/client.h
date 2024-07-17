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

using namespace std;
#pragma comment(lib, "ws2_32.lib")

class Client {
private:
    WSADATA wsaData;
    SOCKET clientSocket;
    sockaddr_in serverAddr;
public:
    Client();
    ~Client();
    void run();
};
#endif // CLIENT_H