#include "fileService.h"

FileService::FileService()
{
}
FileService::~FileService()
{
}

vector<File>& FileService::getFileArr()
{
    return this->fileArr;
}
void FileService::sendFileArr(SOCKET clientSocket)
{
    int fileArrSize = this->fileArr.size();
    //send file size
    sendNumber(clientSocket, fileArrSize);

    for (int i = 0; i < fileArrSize; i++)
    {
        int buffer_size = 0;
        char* buffer = this->fileArr[i].serialize(buffer_size);
        if (!sendBuffer(clientSocket, buffer, buffer_size))
        {
            cout << "Send file frome fileArr failed frome file service class" << endl;
            return;
        }
    }
}
void FileService::receiveFileArr(SOCKET serverSocket)
{
    int fileArrSize = 0;
    //receive file size
    recvNumber(serverSocket, fileArrSize);

    for (int i = 0; i < fileArrSize; i++)
    {
        char* buffer = nullptr;
        if (!recvBuffer(serverSocket, buffer))
        {
            cout << "recv file fail (fileService)" << endl;
            exit(-1);
        }
        File file;
        file.deserialize(buffer);
        this->fileArr.push_back(file);
        delete[] buffer;
    }
}
void FileService::setFileArr()
{
    std::string path = "./file";

    try
    {
        for (const auto& entry : fs::directory_iterator(path))
        {
            if (entry.is_regular_file())
            {
                File file;
                file.setName(entry.path().filename().string());
                file.setSize(entry.file_size());
                this->fileArr.push_back(file);
            }
        }
    }
    catch (const fs::filesystem_error& err)
    {
        std::cerr << "Filesystem error: " << err.what() << std::endl;
    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
    }
    return;
}
void FileService::readUserInput(string fileName)
{
    fileArr.resize(0);
    fstream fi;
    fi.open(fileName.c_str(), ios::in);
    if (!fi.is_open())
    {
        cout << " Can not open file ";
        return;
    }
    string line;
    File file;
    while (getline(fi, line))
    {
        stringstream ss(line);
        string s;
        ss >> s;
        file.setName(s);
        ss >> s;
        if (s == "LOW")
            file.setPriority(FileDowloadPriority::LOW);
        else if (s == "NORMAL")
            file.setPriority(FileDowloadPriority::NORMAL);
        else if (s == "HIGH")
            file.setPriority(FileDowloadPriority::HIGH);
        else if (s == "CRITICAL")
            file.setPriority(FileDowloadPriority::CRITICAL);
        else
            file.setPriority(FileDowloadPriority::NONE);

        fileArr.push_back(file);
    }
    fi.close();
}