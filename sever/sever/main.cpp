#include <winsock2.h>
#include <ws2tcpip.h>
#include <filesystem>
#include <iostream>
#include <cstring>
#include <fstream>
#include "fileService.h"
#include "sever.h"
using namespace std;
#pragma comment(lib, "ws2_32.lib")
int main()
{
    ofstream file;
    file.open("log.txt", ios::out);
    Sever sever;
    sever.run();
    return 0;
}