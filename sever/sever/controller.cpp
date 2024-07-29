#include "controller.h"

Controller::Controller(SOCKET socket)
{
    this->socket = socket;
}
Controller::~Controller()
{
}
void Controller::run()
{
    //  create file service
    FileService fileService;
    // get file array frome computer to file service
    fileService.setFileArr();
    // send file array to client
    fileService.sendFileArr(socket);


    auto start = steady_clock::now();
    auto next_action_time = start; //
    bool stop = false; // flag to stop the loop
    while (!stop)
    {
        auto now = steady_clock::now();

        if (duration_cast<seconds>(now - next_action_time).count() <= 2)
        {
            // system("cls");
            // send flag to client to notify that now the sever will send the file to the client
            bool flag = true;
            send(socket, (char*)&flag, sizeof(flag), 0);
            // do the send file action base on current queue

            //update the queue dowload file


            // send the file to client base on current queue download file and priority


            // update the queue dowload file


            std::this_thread::sleep_for(milliseconds(1000)); // wait a bit to make sure not to call continuously
        }
        else
        {

            // sennd flag to client to notify that the client need to send the file array to sever again
            bool flag = false;
            send(socket, (char*)&flag, sizeof(flag), 0);

            FileService requestFile;
            // recv the file array from client
            requestFile.receiveFileArr(socket);
            // update the queue dowload file
            updateFileQueue(requestFile);

            //testing: print the queue download file
            //clear the screen
            //ansi escape code to clear the screen
            cout << "\033[2J\033[1;1H";
            cout << "file queue" << endl;

            for (int i = 0; i < fileQueue.size(); i++)
            {
                cout << "file " << i << endl;
                cout << "file name " << fileQueue[i].getFile().getName() << endl;
                cout << "file size" << fileQueue[i].getFile().getSize() << endl;
                cout << "file priority" << fileQueue[i].getFile().getPriorityString() << endl;
            }
            next_action_time = steady_clock::now();
        }

        // stop condition
        // if (duration_cast<seconds>(now - start).count() >= 10) {
        //     stop = true;
        // }
        // check if all the file have been sent to the client

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
        }
    }
}

char* Controller::readData(FileProcess fileProcess, int& dataSize){
    fstream fi;
    string fileName = fileProcess.getName();
    fi.open(fileName.c_str(), ios::in | ios::binary);
    if (fi.fail())
        return NULL;

    // tính kích thước file
    fi.seekg(0, ios::end);
    int file_size = fi.tellg();

    // Lấy dữ dataSize của gói tin thứ " process"
    int process = fileProcess.process;
    if ((process + 1) * 512 >= file_size) {
        dataSize = file_size - process * 512;
        if (dataSize >= 512)
            dataSize = 0;
    }
    else
        dataSize = 512;

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

    return buffer;
}


void  Controller::deserializeData(char* bufferData){

    int offset = 0;

    // Lấy độ dài fileName
    int fileNameLength;
    memcpy(&fileNameLength, bufferData, sizeof(fileNameLength));

    // Lấy fileName
    offset += sizeof(fileNameLength);
    char* fileName = new char[fileNameLength];
    memcpy(fileName, bufferData + offset, fileNameLength);

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