#pragma once

#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>

class Server
{
public:
	Server();
	int createServerSocket(int port);
	void respond(int client_fd);
};