#include "fileService.h"

FileService::FileService()
{
}
FileService::~FileService()
{
}

char* FileService::serializeFileArr(int& buffer_size)
{
    vector<char*> serializedFilesArr;
    int fileCount = this->fileArr.size();

    int bufferFileSize = 0;

    for (int i = 0; i < fileCount; i++)
    {
        char* serializedFile = this->fileArr[i].serialize(bufferFileSize);
        serializedFilesArr.push_back(serializedFile);
        buffer_size += bufferFileSize;
    }

    char* buffer = new char[buffer_size];
    int offset = 0;

    // nối lại
    for (int i = 0; i < fileCount; i++)
    {
        int bufferFileSize = 0;
        std::memcpy(&bufferFileSize, serializedFilesArr[i], sizeof(bufferFileSize));
        std::memcpy(buffer + offset, serializedFilesArr[i], bufferFileSize);
        offset += bufferFileSize;
        delete[] serializedFilesArr[i];
    }
    return buffer;
}

vector<File>& FileService::getFileArr()
{
    return this->fileArr;
}
void FileService::sendFileArr(SOCKET clientSocket)
{
    int byteSend = 0;
    int buffer_size = 0;
    char* buffer = serializeFileArr(buffer_size);

    // Send buffer size first
    byteSend += send(clientSocket, (char*)&buffer_size, sizeof(buffer_size), 0);

    // Send the actual buffer
    byteSend += send(clientSocket, buffer, buffer_size, 0);

    delete[] buffer; // Free memory allocated for buffer
    // cout << "byte send " << byteSend << endl;
}
bool FileService::receiveFileArr(SOCKET serverSocket)
{
    int buffer_size;
    char* buffer = new char[sizeof(buffer_size)];

    // Receive the buffer size first
    if (recv(serverSocket, buffer, sizeof(buffer_size), 0) ==  0)
	{
		cout << "Client disconnected" << endl;
        return false;
	}
    std::memcpy(&buffer_size, buffer, sizeof(buffer_size));
    delete[] buffer;

    buffer = new char[buffer_size];
    if (recv(serverSocket, buffer, buffer_size, 0) ==  0)
        {
        cout << "Client disconnected" << endl;
        return false;
		}
    this->fileArr = deserializeFileArr(buffer, buffer_size);
    delete[] buffer;
    return true;
}
std::vector<File> FileService::deserializeFileArr(char* buffer, int buffer_size)
{
    std::vector<File> files;
    int offset = 0;

    while (offset < buffer_size)
    {
        int bufferFileSize = 0;
        std::memcpy(&bufferFileSize, buffer + offset, sizeof(bufferFileSize));
        offset += sizeof(bufferFileSize);

        File file;
        file.deserialize(buffer + offset);
        files.push_back(file);
        // cout << "file name " << file.getName() << endl;
        offset += bufferFileSize - sizeof(int);
    }
    return files;
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