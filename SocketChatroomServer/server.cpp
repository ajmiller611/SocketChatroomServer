﻿#include "server.h"

Server::Server()
{
#ifdef _WIN32
    WSADATA wsa_data;  // empty WSADATA structure
    WORD version_requested = MAKEWORD(2, 2);

    // WSAStartup takes a WSADATA structure and populates the structure with the information
    // needed to implement Windows Sockets. The function returns zero to represent success.
    int wsaerr = WSAStartup(version_requested, &wsa_data);
    if (wsaerr != 0)
    {
        std::cout << "The Winsock dll not found!" << std::endl;
        throw std::runtime_error("Winsock dll not found");
    }
    else
    {
        std::cout << "The Winsock dll found!" << std::endl;
        std::cout << "The status: " << wsa_data.szSystemStatus << std::endl;
    }
#endif
}

int Server::createServerSocket(int port)
{
#ifdef __linux__
    // Create an unbound socket and store the file descriptor in server_fd.
    // AF_INET is the address family specification for Internet Protocol version 4 (IPv4).
    // SOCK_STREAM is the socket type for Transmission Control Protocol (TCP).
    // The third arugment of zero sets it so that the operating system chooses the most
    // appropriate protocol.
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "Failed to create server socket" << std::endl;
        throw std::runtime_error("socket() error");
    }

    //  Set up the sockaddr structure to contain the address and port of the server.
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;  // INADDR_ANY gets the machine's IP address the server is on
    server_addr.sin_port = htons(port);

    // Attempt to bind an unbound socket.
    // server_fd is the file descriptor of the socket.
    // server_addr is the address to be bound to the socket.
    // sizeof(server_addr) is the size of the address structure.
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
    {
        std::cerr << "Failed to bind to port " << port << std::endl;;
        throw std::runtime_error("bind() error");
    }

    int connection_backlog = 5;

    // Attempt to put the socket into a listening state.
    // server_fd is the file descriptor of the socket.
    // connection_backlog is the maximum number of connections that can wait in the queue.
    if (listen(server_fd, connection_backlog) != 0)
    {
        std::cerr << "listen failed" << std::endl;;
        throw std::runtime_error("listen(): Error!");
    }

    return server_fd;

#elif _WIN32
    SOCKET server_socket;
    server_socket = INVALID_SOCKET;

    // Creates a socket to be bound to a specific transport service provider.
    // AF_INET is the address family specification for Internet Protocol version 4 (IPv4).
    // SOCK_STREAM is the socket type for Transmission Control Protocol (TCP).
    // IPPROTO_TCP is the TCP protocol to be used.
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET)
    {
        std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        throw std::runtime_error("socket() error");
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
    // server_socket is the socket descriptor of an unbound socket.
    // service is a pointer to a sockaddr structure that contains the address and port to be bound to the socket.
    // sizeof(service) is the length (in bytes) of the sockaddr structure.
    if (bind(server_socket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        std::cout << "bind() failed: " << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        WSACleanup();
        throw std::runtime_error("bind() error");
    }
    else
    {
        std::cout << "bind() is OK!" << std::endl;
    }

    int connection_backlog = 5;
    // The listen function puts the socket into a state that listens for incoming connections.
    // server_socket is the socket descriptor of a bound and unconnected socket.
    // connection_backlog specifies the backlog of pending connections. The value passed sets
    // the maximum length of the queue.
    if (listen(server_socket, connection_backlog) == SOCKET_ERROR)
    {
        std::cout << "listen(): Error listening on socket " << WSAGetLastError() << std::endl;
        closesocket(server_socket);
        WSACleanup();
        throw std::runtime_error("listen(): Error!");
    }
    else
    {
        std::cout << "listen() is OK, Server is waiting for connections..." << std::endl;
    }

    return (int)server_socket;
#endif
}

void Server::respond(int client_socket)
{
    char receive_buffer[2048];
    char confirmation_buffer[200] = "Message Received";

#ifdef __linux__
    // Attempt to receive data from the connected socket.
    // client_socket is the file descriptor of a connected socket.
    // receive_buffer is a pointer to the data received.
    // sizeof(receive_buffer) is the length (in bytes) of the data received.
    // Zero is the value to represent the flag to use default behavior.
    ssize_t byte_count = recv(client_socket, receive_buffer, sizeof(receive_buffer), 0);
    if (byte_count < 0)
    {
        std::cerr << "Failed to receive request" << std::endl;
    }

    // Attempt to send data to the connected socket.
    // client_socket is the file descriptor of a connected socket.
    // confirmation_buffer is a pointer to the data to be sent.
    // sizeof(confirmation_buffer) is the length (in bytes) of the data being sent.
    // Zero is the value to represent the use of the default flags.
    byte_count = send(client_socket, confirmation_buffer, sizeof(confirmation_buffer), 0);
    if (byte_count < 0)
    {
        std::cerr << "Failed to send response to client" << std::endl;
    }
    close(client_socket);
#elif _WIN32
    // Attempt to receive data from the connected socket.
    // client_socket is the socket descriptor of a connected socket.
    // receive_buffer is a pointer to the data received.
    // sizeof(receive_buffer) is the length (in bytes) of the data received.
    // Zero is the value to represent the flag to use default behavior.
    int byte_count = recv(client_socket, receive_buffer, sizeof(receive_buffer), 0);
    if (byte_count < 0)
    {
        std::cout << "Server receive error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        throw std::runtime_error("Server receive error!");
    }
    else
    {
        std::cout << "Received data: " << receive_buffer << std::endl;
    }

    // Attempt to send data to the connected socket.
    // client_socket is the socket descriptor of a connected socket.
    // confirmation_buffer is a pointer to the data to be sent.
    // sizeof(confirmation_buffer) is the length (in bytes) of the data being sent.
    // Zero is the value to represent the use of the default flags.
    byte_count = send(client_socket, confirmation_buffer, sizeof(confirmation_buffer), 0);
    if (byte_count == SOCKET_ERROR)
    {
        std::cout << "Server send error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        throw std::runtime_error("Server send error!");
    }
    else
    {
        std::cout << "Automated Message sent to Client." << std::endl;
    }

    // Close the client_socket to free up the system resources used.
    closesocket(client_socket);
    std::cout << "Accept socket closed." << std::endl;
#endif
}