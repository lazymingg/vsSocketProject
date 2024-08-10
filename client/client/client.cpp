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
// Initialize the static member
Client* Client::instance = nullptr;

void Client::signalHandler(int signum) {
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    if (instance) {
        instance->closeConnection();
    }
    exit(signum);
}

void Client::closeConnection() {
    if (clientSocket != INVALID_SOCKET) {
        // Inform the peer that no more data will be sent or received
        shutdown(clientSocket, SD_SEND);

        // Close the socket
        closesocket(clientSocket);

        // Clean up Winsock
        WSACleanup();

        std::cout << "Connection closed.\n";

        // Invalidate the socket descriptor
        clientSocket = INVALID_SOCKET;
        exit(1);
    }
}

void Client::run() {
    // Set the instance pointer to this instance
    instance = this;

    // Set up signal handling
    signal(SIGINT, Client::signalHandler);

    // Assuming Controller is another class you have defined
    Controller controller(clientSocket);
    controller.run();
}

Client::~Client()
{
    // Ngắt kết nối
    closesocket(clientSocket);
    WSACleanup();
}