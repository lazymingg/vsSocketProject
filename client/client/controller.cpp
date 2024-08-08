#include "controller.h"

Controller::Controller(SOCKET socket)
{
    this->socket = socket;
    this->progress = 0;
    this->done = false;
}
Controller::~Controller()
{

}
void Controller::updateFileQueue(FileService fileService)
{
    for (File& file : fileService.getFileArr())
    {
        int process = (file.getSize() + 511) / 512; // Simplified calculation
        FileProcess fileProcess(file, process);
        fileQueue.push_back(fileProcess);
    }
}
void Controller::updateDownloadQueue(FileService userRequest)
{
    for (File& file : userRequest.getFileArr())
    {
        for (FileProcess& fileProcess : this->fileQueue)
        {
            if (file.getName() == fileProcess.getFile().getName())
            {
                fileProcess.setNeedTobeDownload(true);
                // update the priority of the file
                fileProcess.getFile().setPriority(file.getPriority());
            }
        }
    }
}
FileService Controller::getRequestFile()
{
    FileService requestFile;
    for (FileProcess& fileProcess : fileQueue)
    {
        if (fileProcess.getNeedTobeDownload())
        {
            requestFile.getFileArr().push_back(fileProcess.getFile());
        }
    }
    return requestFile;
}

void signalHandler(int signum, bool &stop)
{
	cout << "Interrupt signal (" << signum << ") received.\n";
    // close the socket
    stop = true;
	exit(signum);
}

void Controller::run()
{
    // do something
    FileService fileService;
    fileService.receiveFileArr(socket);
    cout << "===================================================" << endl;
    cout << "Here is the file from server: \n";
    for (File file : fileService.getFileArr())
    {
        cout << "File name: " << file.getName() << endl;
        cout << "File size: " << file.getSize() << endl;
    }
    cout << "===================================================" << endl;
    std::cout << "plz modify ur input.txt before we continue press Enter to continue..." << std::endl;
    std::cin.get();

    updateFileQueue(fileService);
    // print file from server

    bool flag = true;

    thread t1(&Controller::printProgressBar, this);

    while (true)
    {

        // print the progress bar
        int byte = recv(socket, (char*)&flag, sizeof(flag), 0);
        if (flag)
        {
            int buffer_size;
            recv(socket, (char*)&buffer_size, sizeof(buffer_size), 0);
            char* buffer = new char[buffer_size];
            int a = recv(socket, buffer, buffer_size, 0);
            deserializeData(buffer);
            delete[] buffer;
            // printProgressBar();
        }
        else
        {
            // read the file frome text file
            FileService requestFile;
            // requestFile.readinput();
            requestFile.readUserInput("input.txt");
            // define what file need to be download
            updateDownloadQueue(requestFile);
            // get the file need to be download
            FileService fileRequest = getRequestFile();
            fileRequest.sendFileArr(socket);
        }
        if (checkStopCondition())
        {
            done = true;
            break;
        }
    }
    t1.join();
    printProgressBarLastTime();
}

void Controller::deserializeData(char* bufferData)
{

    int offset = 0;

    // Lấy độ dài fileName
    int fileNameLength;
    memcpy(&fileNameLength, bufferData, sizeof(fileNameLength));

    // Lấy fileName
    offset += sizeof(fileNameLength);
    char* fileName = new char[fileNameLength + 1];
    memcpy(fileName, bufferData + offset, fileNameLength);
    fileName[fileNameLength] = '\0';
    // Mở file và ghi dữ liệu vào cuối file

    // add path to file
    string path = "./recvFile/";
    path += fileName;
    ofstream fo;

    fo.open(path, ios::binary | ios::app);
    if (fo.fail())
    {
        delete[] fileName;
        return;
    }

    offset += fileNameLength;
    int process;
    memcpy(&process, bufferData + offset, sizeof(process));

    offset += sizeof(process);
    int dataSize;
    memcpy(&dataSize, bufferData + offset, sizeof(dataSize));

    offset += sizeof(dataSize);
    char* data = new char[dataSize];
    memcpy(data, bufferData + offset, dataSize);

    fo.write(data, dataSize);
    fo.close();
    // update the process of the file
    string fileNameStr(fileName);
    updateDowloadProcess(fileNameStr);

    delete[] fileName;
    delete[] data;
}
// increse the process of the file
void Controller::updateDowloadProcess(string fileName)
{
    for (FileProcess& fileProcess : fileQueue)
    {
        if (fileProcess.getFile().getName() == fileName)
        {
            if (fileProcess.getCurrentProcess() != 0)
            {
                fileProcess.setCurrentProcess(fileProcess.getCurrentProcess() - 1);
            }
            return;
        }
    }
}
// check stop condition
bool Controller::checkStopCondition()
{
    for (FileProcess fileProcess : fileQueue)
    {
        if (fileProcess.getCurrentProcess() != 0 && fileProcess.getNeedTobeDownload())
        {
            return false;
        }
    }
    return true;
}

void Controller::printProgressBar()
{
    while (!done)
    {
        // ANSI escape to clear the screen
        cout << "\033[2J\033[1;1H";
        int totalFiles = 0;
        for (FileProcess& fileProcess : fileQueue)
        {
            if (fileProcess.getNeedTobeDownload())
            {
                totalFiles++;
            }
        }

        cout << "Total files to download: " << totalFiles << endl;
        for (FileProcess& fileProcess : fileQueue)
        {
            if (fileProcess.getNeedTobeDownload())
            {
                const string& fileName = fileProcess.getFile().getName();
                double totalProcess = fileProcess.getProcess();
                double currentProcess = fileProcess.getCurrentProcess();

                cout << "File name: " << fileName << endl;
                if (totalProcess > 0)
                {
                    float percentage = float(totalProcess - currentProcess) / float(totalProcess) * 100;
                    cout << "Progress: [" << std::setw(50) << std::setfill(' ') << std::string(int(percentage / 2), '=') << "] " << percentage << "%" << endl;
                }
                else
                {
                    cout << "Progress: [--------------------------------------------------] 0%" << endl;
                }
            }
        }
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

// print the progress bar the last time
void Controller::printProgressBarLastTime()
{
    // ANSI escape to clear the screen
    cout << "\033[2J\033[1;1H";
    int totalFiles = 0;
    for (FileProcess& fileProcess : fileQueue)
    {
        if (fileProcess.getNeedTobeDownload())
        {
            totalFiles++;
        }
    }

    cout << "Total files to download: " << totalFiles << endl;
    for (FileProcess& fileProcess : fileQueue)
    {
        if (fileProcess.getNeedTobeDownload())
        {
            const string& fileName = fileProcess.getFile().getName();
            double totalProcess = fileProcess.getProcess();
            double currentProcess = fileProcess.getCurrentProcess();

            cout << "File name: " << fileName << endl;
            if (totalProcess > 0)
            {
                float percentage = float(totalProcess - currentProcess) / float(totalProcess) * 100;
                cout << "Progress: [" << std::setw(50) << std::setfill(' ') << std::string(int(percentage / 2), '=') << "] " << percentage << "%" << endl;
            }
            else
            {
                cout << "Progress: [--------------------------------------------------] 0%" << endl;
            }
        }
    }
}