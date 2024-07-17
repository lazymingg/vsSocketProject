#include "client.h"

Client::Client()
{
    // Khởi tạo Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "WSAStartup failed with error: " << WSAGetLastError() << endl;

        exit(-1);
    }

    // Tạo một socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        cerr << "Socket creation failed with error: " << WSAGetLastError() << endl;
        WSACleanup();
        exit(-1);
    }

    // Kết nối tới server
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cerr << "Connection failed with error: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        exit(-1);
    }
    cout << "Connected to server!" << endl;
}
void Client::run()
{
    FileService fileService;
    fileService.receiveFileArr(clientSocket);
    for (File file : fileService.getFileArr())
    {
        cout << "File name: " << file.getName() << endl;
        cout << "File size: " << file.getSize() << " bytes" << endl;
    }
}
Client::~Client()
{
    // Ngắt kết nối
    closesocket(clientSocket);
    WSACleanup();
    system("pause");
}