#include "file.h"

// Getter and Setter
string File::getName()
{
    return name;
}

void File::setName(string name)
{
    this->name = name;
}

int64_t File::getSize()
{
    return size;
}

void File::setSize(int64_t size)
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
    buffer_size = sizeof(int) + sizeof(nameSize) + nameSize + sizeof(this->size) + sizeof(this->priority);

    char* buffer = new char[buffer_size];
    // copy data to buffer
    int offset = 0;
    // copy buffer size to buffer
    memcpy(buffer + offset, &buffer_size, sizeof(buffer_size));
    offset += sizeof(buffer_size);
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
    int offset = 0;
    int nameSize;
    // copy name size from buffer
    memcpy(&nameSize, buffer + offset, sizeof(nameSize));
    offset += sizeof(nameSize);
    string name(buffer + offset, nameSize);
    // copy name from buffer
    offset += nameSize;
    int64_t size;
    memcpy(&size, buffer + offset, sizeof(size));
    // copy size from buffer
    offset += sizeof(size);
    FileDowloadPriority priority;
    memcpy(&priority, buffer + offset, sizeof(priority));

    this->name = name;
    this->size = size;
    this->priority = priority;
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