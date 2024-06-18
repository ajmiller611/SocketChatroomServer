#include "server.h"
#include <thread>

int main(int argc, char** argv)
{
    Server server;
    int port = 55555;

#ifdef __linux__
    int server_fd = server.createServerSocket(port);

    while (true) {
        // Set up a sockaddr structure to hold the connected client address.
        struct sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);

        std::cout << "Waiting for a client to connect...\n";

        // Attempt to accept a connection and returns a new file descriptor of a new socket.
        // server_fd is the file descriptor of a socket in a listening state.
        // client_addr is the sockaddr structure with the client address information.
        // client_addr_len is the size of the sockaddr structure.
        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, (socklen_t*)&client_addr_len);
        if (client_fd < 0) {
            std::cerr << "Failed to accept connection from client\n";
            return 1;
        }
        std::cout << "Client connected\n";

        std::thread worker(&Server::respond, &server, client_fd);
        worker.detach();
    }
    close(server_fd);

#elif _WIN32
    SOCKET server_socket, accept_socket;
    server_socket = server.createServerSocket(port);

    while (true)
    {
        // Attempts to accept an incoming connection.
        // server_socket is the socket descriptor of a socket that is in a listening state.
        // The second parameter is an optional pointer to a sockaddr structure that will contain
        // the client address information.
        // The third parameter is length of the sockaddr structure of the second parameter.
        accept_socket = accept(server_socket, NULL, NULL);
        if (accept_socket == INVALID_SOCKET)
        {
            std::cout << "accept failed: " << WSAGetLastError() << std::endl;
            WSACleanup();
            return -1;
        }
        std::cout << "Accepted connection" << std::endl;

        std::thread worker(&Server::respond, &server, (int)accept_socket);
        worker.detach();
    }
    std::cout << "Server shutting down..." << std::endl;

    // Close the server_socket to free up the system resources used.
    closesocket(server_socket);
    std::cout << "Server socket closed." << std::endl;

    // WSACleanup function frees up the system resources used from the calling of the WSAStartup function.
    WSACleanup();
    std::cout << "Clean up done. Exiting program." << std::endl;

#endif

    return 0;
}