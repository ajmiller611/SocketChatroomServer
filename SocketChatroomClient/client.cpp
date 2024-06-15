#include "client.h"

int main(int argc, char** argv)
{
	SOCKET clientSocket;
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

	clientSocket = INVALID_SOCKET;

	// Creates a socket to be bound to a specific transport service provider.
	// AF_INET is the address family specification for Internet Protocol version 4 (IPv4).
	// SOCK_STREAM is the socket type for Transmission Control Protocol (TCP).
	// IPPROTO_TCP is the TCP protocol to be used.
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET)
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
	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &clientService.sin_addr.s_addr);
	clientService.sin_port = htons(port);

	// Attempt to connect to a specified socket. Also, the connect function takes care of binding the socket.
	// clientSocket is the socket descriptor of the socket to use, in this case, on the client side of the connection.
	// clientService is a pointer to a sockaddr structure that contains the address and port of the server socket to connect to.
	// sizeof(clientService) is the length (in bytes) of the sockaddr structure.
	if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
	{
		std::cout << "Client: connect() - Failed to connect." << std::endl;
		WSACleanup();
		return 0;
	}
	else
	{
		std::cout << "Client: connect() is OK." << std::endl;
		std::cout << "Client: Can start sending and receiving data..." << std::endl;
	}

	char buffer[200];
	bool flag = true;
	while (flag)
	{
		std::cout << "Enter a message: ";
		std::cin.getline(buffer, 200);

		// Attempt to send data to the connected socket.
		// clientSocket is the socket descriptor of a connected socket.
		// buffer is a pointer to the data to be sent.
		// sizeof(buffer) is the length (in bytes) of the data being sent.
		// Zero is the value to represent the use of the default flags. 
		int byteCount = send(clientSocket, buffer, sizeof(buffer), 0);
		if (byteCount == SOCKET_ERROR)
		{
			std::cout << "Client send error: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return -1;
		}
		else
		{
			std::cout << "Client sent " << byteCount << " bytes." << std::endl;
		}

		if (strcmp(buffer, "SHUTDOWN") == 0)
		{
			flag = false;
			std::cout << "Shutdown code sent to server." << std::endl;
		}

		// Attempt to receive data from the connected socket.
		// clientSocket is the socket descriptor of a connected socket.
		// buffer is a pointer to the data received.
		// sizeof(buffer) is the length (in bytes) of the data received.
		// Zero is the value to represent the flag to use default behavior.
		byteCount = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (byteCount < 0)
		{
			std::cout << "Client receive error: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return 0;
		}
		else
		{
			std::cout << "Message received: " << buffer << std::endl;
		}
	}
	std::cout << "Client shutting down..." << std::endl;

	// Close the socket to free up the system resources used.
	closesocket(clientSocket);

	// WSACleanup function frees up the system resources used from the calling of the WSAStartup function.
	WSACleanup();
	std::cout << "Clean up done. Exiting program." << std::endl;
	return 0;
}