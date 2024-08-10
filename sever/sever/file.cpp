#include "file.h"

// // Getter and Setter
string File::getName()
{
    return name;
}

void File::setName(string name)
{
    this->name = name;
}

long long File::getSize()
{
    return size;
}

void File::setSize(long long size)
{
    this->size = size;
}

FileDowloadPriority File::getPriority()
{
    return priority;
}

void File::setPriority(FileDowloadPriority priority)
{
    this->priority = priority;
}

// Serialize File object to a buffer
char* File::serialize(int& buffer_size)
{

    int nameSize = this->name.size();
    buffer_size = sizeof(nameSize) + nameSize + sizeof(this->size) + sizeof(this->priority);

    char* buffer = new char[buffer_size];
    // copy data to buffer
    int offset = 0;
    // copy name size to buffer
    memcpy(buffer + offset, &nameSize, sizeof(nameSize));
    offset += sizeof(nameSize);
    // copy name to buffer
    memcpy(buffer + offset, this->name.c_str(), nameSize);
    offset += nameSize;
    // copy size to buffer
    memcpy(buffer + offset, &this->size, sizeof(this->size));
    offset += sizeof(this->size);
    // copy priority to buffer
    memcpy(buffer + offset, &this->priority, sizeof(this->priority));
    return buffer;
}

void File::deserialize(char* buffer)
{
    int offSet = 0;
    int nameSize;
    // copy name size from buffer
    memcpy(&nameSize, buffer + offSet, sizeof(nameSize));
    offSet += sizeof(nameSize);

    // copy name from buffer
    char* name = new char[nameSize + 1];
    memcpy(name, buffer + offSet, nameSize);
    name[nameSize] = '\0';
    this->name = name;
    offSet += nameSize;

    // copy size from buffer
    memcpy(&this->size, buffer + offSet, sizeof(this->size));
    offSet += sizeof(this->size);
    // copy priority from buffer
    memcpy(&this->priority, buffer + offSet, sizeof(this->priority));
    delete[] name;
}

string File::getPriorityString()
{
    switch (this->priority)
    {
    case FileDowloadPriority::LOW:
        return "LOW";
    case FileDowloadPriority::NORMAL:
        return "NORMAL";
    case FileDowloadPriority::HIGH:
        return "HIGH";
    case FileDowloadPriority::CRITICAL:
        return "CRITICAL";
    default:
        return "NONE";
    }
}
