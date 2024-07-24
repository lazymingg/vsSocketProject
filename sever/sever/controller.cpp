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