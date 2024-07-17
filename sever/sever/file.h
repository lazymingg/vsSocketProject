#ifndef FILE_H
#define FILE_H

// necessary libraries or headers here
#include <fstream>
#include <cstring>
#include <iostream>
#include <filesystem>
#include <cstdint>
using namespace std;

// functions, classes, or variables here

enum class FileDowloadPriority
{
    LOW,
    NORMAL,
    HIGH,
    CRITICAL,
    NONE
};

class File
{
private:
    string name = "";
    int64_t size = 0;
    FileDowloadPriority priority = FileDowloadPriority::NONE;

public:
    string getName();

    int64_t getSize();

    FileDowloadPriority getPriority();

    void setName(string name);

    void setSize(int64_t size);

    void setPriority(FileDowloadPriority priority);

    char* serialize(int& buffer_size);

    void deserialize(char* buffer);
};

#endif // FILE_H