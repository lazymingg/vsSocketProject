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
    int maxProcess = 0;
    bool isDone = false;


    FileProcess()
    {
        this->file = File();
        this->process = 0;
    }

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
    void updateMaxProcess()
    {
        if (this->file.getSize() % 512 == 0)
        {
            this->maxProcess = this->file.getSize() / 512;
        }
        else
        {
            this->maxProcess = this->file.getSize() / 512 + 1;
        }
    }
};

class Controller
{
public:
    Controller(SOCKET socket);
    ~Controller();
    void run();
    void updateFileQueue(FileService requestFile);
    char* readData(FileProcess fileProcess, int& dataSize);
    char* serializeData(FileProcess fileProcess, int& bufferSize);
    void deserializeData(char* bufferData);
    int downloadSpeed(FileProcess fileProcess);
    char* getDataChunk(FileProcess& fileProcess, int& bufferSize);
    queue<FileProcess> createDowloadLine();
    bool isAllFileDone();
private:
    vector<FileProcess> fileQueue;
    SOCKET socket;
    // Declare any private member variables or functions here
};
// Declare your class or function prototypes here

#endif // CONTROLLER_H