#ifndef FILE_H
#define FILE_H

// necessary libraries or headers here
#include <iostream>
#include <cstring>
#include <string>
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
    long long size = 0;
    FileDowloadPriority priority = FileDowloadPriority::NONE;
public:
    string getName();

    long long getSize();

    FileDowloadPriority getPriority();

    string getPriorityString();

    void setPriority(FileDowloadPriority priority);

    void setName(string name);

    void setSize(long long size);

    char* serialize(int& buffer_size);

    void deserialize(char* buffer);
};

#endif // FILE_H