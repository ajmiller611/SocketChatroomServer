#pragma once

#include <iostream>
#include <string>

#ifdef __linux__
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <unistd.h>
	#include <arpa/inet.h>
	#include <netdb.h>
#elif _WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <tchar.h>
#endif 

class Server
{
public:
	Server();
	int createServerSocket(int port);
	void respond(int client_fd);
};