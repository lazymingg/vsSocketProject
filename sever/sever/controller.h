#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <iostream>

#include <winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#include <queue>
#include <thread>
#include <chrono>
#include <vector>
#include <string>
#include <sstream>

#include "fileService.h"
using namespace std;
using namespace chrono;
// Include any necessary libraries or headers here
struct FileProcess
{
    File file;
    int process = 0;

    FileProcess(File file, int process)
    {
        this->file = file;
        this->process = process;
    }

    string getName()
    {
        return this->file.getName();
    }

    File getFile()
    {
        return file;
    }
};

class Controller
{
public:
    Controller(SOCKET socket);
    ~Controller();
    void run();
    void updateFileQueue(FileService requestFile);
private:
    vector<FileProcess> fileQueue;
    SOCKET socket;
    // Declare any private member variables or functions here
};
// Declare your class or function prototypes here

#endif // CONTROLLER_H