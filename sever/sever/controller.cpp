#include "controller.h"

Controller::Controller(SOCKET socket)
{
    this->socket = socket;
}
Controller::~Controller()
{
}
bool checkClientShutdown(int recvCode)
{
    if (recvCode == SOCKET_ERROR)
    {
        int errorCode = WSAGetLastError();
        if (errorCode == WSAECONNRESET) {
            return false;
        }
        else if (errorCode == WSAENOTCONN) {
            return false;
        }
        else {
            return false;
        }
        // Xử lý lỗi hoặc dọn dẹp tài nguyên
    }
    return true;
}
void Controller::run()
{
    //  create file service
    FileService fileService;
    // get file array frome computer to file service
    fileService.setFileArr();
    // send file array to client
    fileService.sendFileArr(socket);

    vector <FileProcess> fileDownloadQueue;

    auto start = steady_clock::now();
    auto next_action_time = start; //
    bool stop = false; // flag to stop the loop


    int currentFile = 0;
    int maxFile = 0;
    // loop to send file to client

    bool theFirst = true;
   
    while (!stop)
    {
        auto now = steady_clock::now();

        if (duration_cast<seconds>(now - next_action_time).count() <= 2 && !theFirst)
        {
            // system("cls");
            // send flag to client to notify that now the sever will send the file to the client
            bool flag = true;
            int byteSend = send(socket, (char*)&flag, sizeof(flag), 0);
            if (!checkClientShutdown(byteSend))
            {
                cout << "client disconnected" << endl;
                stop = true;
                break;
            }
            if (currentFile == maxFile)
            {
                fileDownloadQueue = createDowloadLine();
                maxFile = fileDownloadQueue.size();
                currentFile = 0;
            }

            int bufferSize = 0;
            char* buffer = getDataChunk(fileDownloadQueue[currentFile], bufferSize);
            // send the buffer size to client
            byteSend = send(socket, (char*)&bufferSize, sizeof(bufferSize), 0);
            if (!checkClientShutdown(byteSend))
            {
                cout << "client disconnected" << endl;
                stop = true;
                break;
            }
            // send the buffer to
            byteSend = send(socket, buffer, bufferSize, 0);
            if (!checkClientShutdown(byteSend))
            {
                cout << "client disconnected" << endl;
                stop = true;
                break;
            }
            currentFile++;
            if (currentFile == maxFile)
            {
                fileDownloadQueue = createDowloadLine();
                maxFile = fileDownloadQueue.size();
                currentFile = 0;
            }

            delete[] buffer;

            // std::this_thread::sleep_for(milliseconds(0)); // wait a bit to make sure not to call continuously
        }
        else
        {
            // sennd flag to client to notify that the client need to send the file array to sever again
            bool flag = false;
            int byteSend = send(socket, (char*)&flag, sizeof(flag), 0);
            if (!checkClientShutdown(byteSend))
            {
				cout << "client disconnected" << endl;
				stop = true;
				break;
			}
            FileService requestFile;
            // recv the file array from client
            if (!requestFile.receiveFileArr(socket))
            {
                cout << "client disconnected" << endl;
                stop = true;
				break;
            }
            // update the queue dowload file
            updateFileQueue(requestFile);
            // tesing the file queue
            next_action_time = steady_clock::now();
            theFirst = false;
        }

        // stop condition
        if (maxFile == 0 && isAllFileDone() && !theFirst)
        {
            cout << "all file have been sent to the client" << endl;
            stop = true;
            break;
        }
        // check if all the file have been sent to the client

    }
}
bool Controller::isAllFileDone()
{
    int size = fileQueue.size();
    for (int i = 0; i < size; i++)
    {
        if (!fileQueue[i].isDone)
        {
            return false;
        }
    }
    return true;
}
vector<FileProcess> Controller::createDowloadLine()
{
    vector<FileProcess> fileDownloadQueue;
    int size = fileQueue.size();
    for (int i = 0; i < size; i++)
    {
        if (fileQueue[i].isDone)
        {
            continue;
        }
        for (int j = 0; j < downloadSpeed(fileQueue[i]); j++)
        {
            fileDownloadQueue.push_back(fileQueue[i]);
            fileQueue[i].process++;
            if (fileQueue[i].process == fileQueue[i].maxProcess)
            {
                fileQueue[i].isDone = true;
                break;
            }
        }
    }
    return fileDownloadQueue;
}
// send file for client 
char* Controller::getDataChunk(FileProcess& fileProcess, int& buffer_size)
{
    if (fileProcess.isDone)
    {
        return NULL;
    }
    int bufferSize = 0;
    char* buffer = serializeData(fileProcess, bufferSize);
    buffer_size = bufferSize;

    fileProcess.process++;
    if (fileProcess.process == fileProcess.maxProcess)
    {
        fileProcess.isDone = true;
    }
    return buffer;
}

//dowload speed
int Controller::downloadSpeed(FileProcess fileProcess)
{
    switch (fileProcess.getFile().getPriority())
    {
    case FileDowloadPriority::LOW:
        return 1;
    case FileDowloadPriority::NORMAL:
        return 5;
    case FileDowloadPriority::HIGH:
        return 15;
    case FileDowloadPriority::CRITICAL:
        return 50;
    default:
        return 0;
    }
}
void Controller::updateFileQueue(FileService requestFile)
{
    // update the queue download file
    int size = requestFile.getFileArr().size();
    int sizeQueue = fileQueue.size();
    for (int i = 0; i < size; i++)
    {

        // check if the file is already in the queue
        bool isExist = false;
        for (int j = 0; j < sizeQueue; j++)
        {
            if (requestFile.getFileArr()[i].getName() == this->fileQueue[j].getName())
            {
                //  update the file in the queue
                this->fileQueue[j].file = requestFile.getFileArr()[i];
                isExist = true;
            }
        }
        if (!isExist)
        {
            // add the file to the queue
            FileProcess fileProcess(requestFile.getFileArr()[i], 0);
            this->fileQueue.push_back(fileProcess);
            sizeQueue++;
        }
    }
    //update the process of the file in the queue
    for (int i = 0; i < sizeQueue; i++)
    {
        this->fileQueue[i].updateMaxProcess();
    }
}

char* Controller::readData(FileProcess fileProcess, int& dataSize) {
    fstream fi;
    string fileName = fileProcess.getName();
    // Path to file
    string path = "./file/" + fileName;
    fi.open(path.c_str(), ios::in | ios::binary);
    if (fi.fail()) {
        dataSize = 0;
        return nullptr;
    }

    // Calculate file size
    fi.seekg(0, ios::end);
    int file_size = fi.tellg();

    // Get dataSize of the packet "process"
    int process = fileProcess.process;
    if ((process + 1) * 512 >= file_size) {
        dataSize = file_size - process * 512;
        if (dataSize <= 0) {
            dataSize = 0;
            fi.close();
            return nullptr;
        }
    }
    else {
        dataSize = 512;
    }

    char* data = new char[dataSize];
    fi.seekg(process * 512, ios::beg);
    fi.read(data, dataSize);
    fi.close();

    return data;
}
char* Controller::serializeData(FileProcess fileProcess, int& bufferSize) {

    string fileName = fileProcess.getName();
    int fileNameLength = fileName.size();
    int process = fileProcess.process;
    int dataSize;

    // Lấy data và dataSize
    char* data = readData(fileProcess, dataSize);

    bufferSize = sizeof(fileNameLength) + fileNameLength + sizeof(process)
        + sizeof(dataSize) + dataSize;

    char* buffer = new char[bufferSize];
    int offset = 0;

    // lưu độ dài fileName
    memcpy(buffer + offset, &fileNameLength, sizeof(fileNameLength));
    offset += sizeof(fileNameLength);

    // lưu fileName
    memcpy(buffer + offset, fileName.c_str(), fileNameLength);
    offset += fileNameLength;

    // Lưu biến process
    memcpy(buffer + offset, &process, sizeof(process));
    offset += sizeof(process);

    // Lưu dataSize
    memcpy(buffer + offset, &dataSize, sizeof(dataSize));
    offset += sizeof(dataSize);

    // Lưu Data
    memcpy(buffer + offset, data, dataSize);
    delete[] data;
    return buffer;
}


void  Controller::deserializeData(char* bufferData)
{

    int offset = 0;

    // Lấy độ dài fileName
    int fileNameLength;
    memcpy(&fileNameLength, bufferData, sizeof(fileNameLength));

    // Lấy fileName
    offset += sizeof(fileNameLength);
    char* fileName = new char[fileNameLength + 1];
    memcpy(fileName, bufferData + offset, fileNameLength);

    // add null-terminator
    fileName[fileNameLength] = '\0';
    // Mở file và ghi dữ liệu vào cuối file

    ofstream fo;
    fo.open(fileName, ios::binary | ios::app);
    if (fo.fail()) {
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

    delete[] fileName;
    delete[] data;
}