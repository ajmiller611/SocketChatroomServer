#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

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
	struct HttpRequest {
		std::string action;
		std::string path;
		std::string version;
		std::unordered_map<std::string, std::string> headers;
		std::string body;
	};

	struct HttpResponse {
		std::string status_line;
		std::unordered_map<std::string, std::string> headers;
		std::string body;
	};

	std::vector<int> m_accepted_connections;

public:
	Server();
	int createServerSocket(int port);
	void addAcceptedSocket(int socket);
	HttpRequest parseRequest(const std::string& requestStr);
	std::string readFile(const std::string& file_path);
	HttpResponse requestGET(HttpRequest request);
	void respond(int client_fd);
};