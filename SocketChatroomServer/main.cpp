#include "server.h"

int main(int argc, char** argv)
{
    Server server;

    SOCKET serverSocket, acceptSocket;
    int port = 55555;
    serverSocket = server.createServerSocket(port);

    while (true)
    {
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

        server.respond(acceptSocket);
    }
    std::cout << "Server shutting down..." << std::endl;

    // Close the serverSocket to free up the system resources used.
    closesocket(serverSocket);
    std::cout << "Server socket closed." << std::endl;

    // WSACleanup function frees up the system resources used from the calling of the WSAStartup function.
    WSACleanup();
    std::cout << "Clean up done. Exiting program." << std::endl;

    return 0;
}