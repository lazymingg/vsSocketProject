#ifndef SEVER_H
#define SEVER_H
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <filesystem>
#include <thread>
#include <chrono>
#include "fileService.h"
#include "controller.h"
using namespace std;


#define DEFAULT_BUFLEN 512


class Sever {
private:
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);
public:
    Sever();
    ~Sever();
    void run();
};
// Add your code here

#endif // SEVER_H