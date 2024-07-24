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
    // do something
    FileService fileService;
    fileService.receiveFileArr(socket);
    for (File file : fileService.getFileArr())
    {
        cout << "File name: " << file.getName() << endl;
        cout << "File size: " << file.getSize() << " bytes" << endl;
        cout << "File priority: " << file.getPriorityString() << endl;
    }

    bool flag = true;
    while (true)
    {
        int byte = recv(socket, (char*)&flag, sizeof(flag), 0);
        if (flag)
        {
            cout << "recv file" << endl;
        }
        else
        {
            // read the file frome text file
            FileService requestFile;
            //requestFile.readinput();
            requestFile.readUserInput("input.txt");
            // send the file to client
            requestFile.sendFileArr(socket);
        }
    }
}