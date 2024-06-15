#include "server.h"

int main(int argc, char** argv)
{
    SOCKET serverSocket, acceptSocket;
    int port = 55555;
    WSADATA wsaData;  // empty WSADATA structure
    WORD wVersionRequested = MAKEWORD(2, 2);

    // WSAStartup takes a WSADATA structure and populates the structure with the information
    // needed to implement Windows Sockets. The function returns zero to represent success.
    int wsaerr = WSAStartup(wVersionRequested, &wsaData);
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

    // Creates a socket to be bound to a specific transport service provider.
    // AF_INET is the address family specification for Internet Protocol version 4 (IPv4).
    // SOCK_STREAM is the socket type for Transmission Control Protocol (TCP).
    // IPPROTO_TCP is the TCP protocol to be used.
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

    // Set up the sockaddr structure to contain the address and port of the server.
    sockaddr_in service;
    service.sin_family = AF_INET;
    InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr);
    service.sin_port = htons(port);

    // Attempt to bind an unbound socket.
    // serverSocket is the socket descriptor of an unbound socket.
    // service is a pointer to a sockaddr structure that contains the address and port to be bound to the socket.
    // sizeof(service) is the length (in bytes) of the sockaddr structure.
    if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) 
    {
        std::cout << "bind() failed: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 0;
    }
    else 
    {
        std::cout << "bind() is OK!" << std::endl;
    }

    // The listen function puts the socket into a state that listens for incoming connections.
    // serverSocket is the socket descriptor of a bound and unconnected socket.
    // The second parameter specifies the backlog of pending connections. The value passed sets
    // the maximum length of the queue.
    if (listen(serverSocket, 1) == SOCKET_ERROR)
    {
        std::cout << "listen(): Error listening on socket " << WSAGetLastError() << std::endl;
    }
    else 
    {
        std::cout << "listen() is OK, I'm waiting for connections..." << std::endl;
    }

    // Attempts to accept an incoming connection.
    // serverSocket is the socket descriptor of a socket that is in a listening state.
    // The second parameter is an optional pointer to a sockaddr structure that will contain
    // the client address information.
    // The third parameter is length of the sockaddr structure of the second parameter.
    acceptSocket = accept(serverSocket, NULL, NULL);
    if (acceptSocket == INVALID_SOCKET)
    {
        std::cout << "accept failed: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }
    std::cout << "Accepted connection" << std::endl;

    char receiveBuffer[2048];
    while (true)
    {
        // Attempt to receive data from the connected socket.
        // acceptSocket is the socket descriptor of a connected socket.
        // receiveBuffer is a pointer to the data received.
        // sizeof(receiveBuffer) is the length (in bytes) of the data received.
        // Zero is the value to represent the flag to use default behavior.
        int byteCount = recv(acceptSocket, receiveBuffer, sizeof(receiveBuffer), 0);
        if (byteCount < 0)
        {
            std::cout << "Server receive error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 0;
        }
        else
        {
            std::cout << "Received data: " << receiveBuffer << std::endl;
        }

        char confirmationBuffer[200] = "Message Received";

        // Attempt to send data to the connected socket.
        // acceptSocket is the socket descriptor of a connected socket.
        // confirmationBuffer is a pointer to the data to be sent.
        // sizeof(confirmationBuffer) is the length (in bytes) of the data being sent.
        // Zero is the value to represent the use of the default flags.
        byteCount = send(acceptSocket, confirmationBuffer, sizeof(confirmationBuffer), 0);
        if (byteCount == SOCKET_ERROR)
        {
            std::cout << "Server send error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return -1;
        }
        else
        {
            std::cout << "Automated Message sent to Client." << std::endl;
        }
    }
    std::cout << "Server shutting down..." << std::endl;

    // Close the acceptSocket to free up the system resources used.
    closesocket(acceptSocket);
    std::cout << "Accept socket closed." << std::endl;

    // Close the serverSocket to free up the system resources used.
    closesocket(serverSocket);
    std::cout << "Server socket closed." << std::endl;

    // WSACleanup function frees up the system resources used from the calling of the WSAStartup function.
    WSACleanup();
    std::cout << "Clean up done. Exiting program." << std::endl;

	return 0;
}
