#ifndef UTILITY_H
#define UTILITY_H
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
using namespace std;

bool sendBuffer(SOCKET clientSocket, char* buffer, int buffer_size);
bool recvBuffer(SOCKET clientSocket, char*& buffer);
int sendNumber(SOCKET clientSocket, int number);
int recvNumber(SOCKET clientSocket, int& number);

#endif // UTILITY_H