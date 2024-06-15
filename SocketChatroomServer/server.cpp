#include "server.h"

int main(int arvc, char** argv)
{
    SOCKET serverSocket;
    int port = 55555;
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaerr != 0)
    {
        std::cout << "The Winsock dll not found!" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "The Winsock dll found!" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;
    }

    serverSocket = INVALID_SOCKET;
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 0;
    }
    else 
    {
        std::cout << "socket() is OK!" << std::endl;
    }

    sockaddr_in service;
    service.sin_family = AF_INET;
    InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr);
    service.sin_port = htons(port);
    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        std::cout << "bind() failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }
    else {
        std::cout << "bind() is OK!" << std::endl;
    }

    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        std::cout << "listen(): Error listening on socket " << WSAGetLastError() << std::endl;
    }
    else {
        std::cout << "listen() is OK, I'm waiting for connections..." << std::endl;
    }
    system("pause");
    closesocket(serverSocket);
    WSACleanup();

	return 0;
}
