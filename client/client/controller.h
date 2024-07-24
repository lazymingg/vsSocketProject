#ifndef CONTROLLER_H
#define CONTROLLER_H
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "file.h"
#include <queue>
#include <thread>
#include <chrono>
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

};

class Controller
{
public:
    Controller(SOCKET socket);
    ~Controller();
    void run();
private:
    queue<File> fileQueue;
    SOCKET socket;
    // Declare any private member variables or functions here
};
// Declare your class or function prototypes here

#endif // CONTROLLER_H