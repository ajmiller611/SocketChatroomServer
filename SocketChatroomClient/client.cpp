#include "client.h"

void receiveMessages(int client_socket)
{
	char buffer[2048];
	while (true)
	{
		// Clear the buffer before each use.
		memset(buffer, '\0', sizeof(buffer));

		// Attempt to receive data from the connected socket.
		// client_socket is the socket descriptor of a connected socket.
		// buffer is a pointer to the data received.
		// sizeof(buffer) is the length (in bytes) of the data received.
		// Zero is the value to represent the flag to use default behavior.
		int byte_count = recv(client_socket, buffer, sizeof(buffer), 0);
		if (byte_count < 0)
		{
			std::cout << "Client receive error: " << WSAGetLastError() << std::endl;
			WSACleanup();
			throw std::runtime_error("Client receive error!");
		}
		else
		{
			std::cout << "Message received: " << buffer << std::endl;
		}
	}
}

int main(int argc, char** argv)
{
	SOCKET client_socket;
	int port = 55555;
	WSADATA wsa_data;  // empty WSADATA structure
	WORD version_requested = MAKEWORD(2, 2);

	// WSAStartup takes a WSADATA structure and populates the structure with the information
	// needed to implement Windows Sockets. The function returns zero to represent success.
	int wsaerr = WSAStartup(version_requested, &wsa_data);
	if (wsaerr != 0)
	{
		std::cout << "The Winsock dll not found!" << std::endl;
		return 0;
	}
	else
	{
		std::cout << "The Winsock dll found!" << std::endl;
		std::cout << "The status: " << wsa_data.szSystemStatus << std::endl;
	}

	client_socket = INVALID_SOCKET;

	// Creates a socket to be bound to a specific transport service provider.
	// AF_INET is the address family specification for Internet Protocol version 4 (IPv4).
	// SOCK_STREAM is the socket type for Transmission Control Protocol (TCP).
	// IPPROTO_TCP is the TCP protocol to be used.
	client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_socket == INVALID_SOCKET)
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
	sockaddr_in client_service;
	client_service.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &client_service.sin_addr.s_addr);
	client_service.sin_port = htons(port);

	// Attempt to connect to a specified socket. Also, the connect function takes care of binding the socket.
	// client_socket is the socket descriptor of the socket to use, in this case, on the client side of the connection.
	// client_service is a pointer to a sockaddr structure that contains the address and port of the server socket to connect to.
	// sizeof(client_service) is the length (in bytes) of the sockaddr structure.
	if (connect(client_socket, (SOCKADDR*)&client_service, sizeof(client_service)) == SOCKET_ERROR)
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

	std::thread worker(receiveMessages, client_socket);

	char buffer[2048];
	bool flag = true;
	while (flag)
	{
		// Clear the buffer before each use.
		memset(buffer, '\0', sizeof(buffer));
		std::cout << "Enter a message: ";
		std::cin.getline(buffer, 2048);

		// Attempt to send data to the connected socket.
		// client_socket is the socket descriptor of a connected socket.
		// buffer is a pointer to the data to be sent.
		// sizeof(buffer) is the length (in bytes) of the data being sent.
		// Zero is the value to represent the use of the default flags. 
		int byte_count = send(client_socket, buffer, sizeof(buffer), 0);
		if (byte_count == SOCKET_ERROR)
		{
			std::cout << "Client send error: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return -1;
		}
		else
		{
			std::cout << "Client sent " << byte_count << " bytes." << std::endl;
		}

		if (strcmp(buffer, "SHUTDOWN") == 0)
		{
			flag = false;
			std::cout << "Shutdown code sent to server." << std::endl;
		}
	}
	std::cout << "Client shutting down..." << std::endl;

	// Close the socket to free up the system resources used.
	closesocket(client_socket);

	// WSACleanup function frees up the system resources used from the calling of the WSAStartup function.
	WSACleanup();
	std::cout << "Clean up done. Exiting program." << std::endl;
	return 0;
}