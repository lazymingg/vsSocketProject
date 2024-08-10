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
#include <iomanip> // For std::setw
#include <atomic>
#include <mutex>
#include <condition_variable>
using namespace std;
using namespace chrono;
// Include any necessary libraries or headers here
struct FileProcess
{
    File file;
    int process = 0;
    int currentProcess = 0;
    bool needTobeDownload = false;
    bool downloaded = false;
    FileProcess(File file, int process)
    {
        this->file = file;
        this->process = process;
        this->currentProcess = process;
    }
    //getters
    File& getFile()
    {
        return file;
    }
    int getProcess()
    {
        return process;
    }
    int getCurrentProcess()
    {
        return currentProcess;
    }
    bool getNeedTobeDownload()
    {
        return needTobeDownload;
    }
    //setters
    void setNeedTobeDownload(bool needTobeDownload)
    {
        this->needTobeDownload = needTobeDownload;
    }
    void setFile(File file)
    {
        this->file = file;
    }
    void setProcess(int process)
    {
        this->process = process;
    }
    void setCurrentProcess(int currentProcess)
    {
        this->currentProcess = currentProcess;
    }


};

class Controller
{
public:
    Controller(SOCKET socket);
    ~Controller();
    void run();
    std::atomic<float> progress;
    std::atomic<bool> done;
private:
    vector<FileProcess> fileQueue;
    SOCKET socket;
    FileService fileNotExist;


    FileService getRequestFile();
    void deserializeData(char* bufferData);
    void updateFileQueue(FileService fileService);
    void updateDownloadQueue(FileService userRequest);
    void updateDowloadProcess(string fileName);
    bool checkStopCondition();
    void printProgressBar();
    void printProgressBarLastTime();
    // Declare any private member variables or functions here
};

#endif // CONTROLLER_H