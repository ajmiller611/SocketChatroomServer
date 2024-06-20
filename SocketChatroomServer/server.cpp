#include "server.h"
#include <fstream>
#include <sstream>

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

void Server::addAcceptedSocket(int socket)
{
    m_accepted_connections.push_back(socket);
}

Server::HttpRequest Server::parseRequest(const std::string& request_str)
{
    HttpRequest http_request;
    std::stringstream request_stream(request_str);
    std::string line;
    std::getline(request_stream, line);
    std::stringstream line_stream(line);
    line_stream >> http_request.action >> http_request.path >> http_request.version;

    while (std::getline(request_stream, line) && line != "\r")
    {
        int colon_pos = line.find(':');
        if (colon_pos != std::string::npos) {
            std::string header_name = line.substr(0, colon_pos);
            std::string value = line.substr(colon_pos + 2); // Add 2 to move index past the colon and space
            value.erase(remove(value.begin(), value.end(), '\r'), value.end()); // remove hidden carriage returns
            std::string header_value = value;
            http_request.headers[header_name] = header_value;
        }
    }

    while (std::getline(request_stream, line))
    {
        http_request.body += line;
    }

    return http_request;
}

std::string Server::readFile(const std::string& file_path)
{
    const std::string WEBSITE_ROOT_DIR = "C:/Users/ajmil/source/repos/SocketChatroomServer/WebsiteRoot/";
    std::ifstream file(WEBSITE_ROOT_DIR + "index.html");
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + WEBSITE_ROOT_DIR + "index.html");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

Server::HttpResponse Server::requestGET(HttpRequest request)
{
    HttpResponse response;
    if (request.path == "/" || request.path == "/index.html") {
        try
        {
            std::string file_content = readFile("index.html");
            response.status_line = "HTTP/1.1 200 OK";
            response.headers["Content-Type"] = "text/html";
            response.headers["Content-Length"] = std::to_string(file_content.size());
            response.body = file_content;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            response.status_line = "HTTP/1.1 404 Not Found";
            response.body = "<html><body><h1>404 Not Found</h1></body></html>";
        }
    }
    else
    {
        response.status_line = "HTTP/1.1 404 Not Found";
    }
    return response;
}

void Server::respond(int client_socket)
{
    char receive_buffer[2048];

#ifdef __linux__
    while (true)
    {
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
        else if (byte_count == 0)
        {
            std::cerr << "Client disconnected" << std::endl;
            break;
        }

        std::string buffer(receive_buffer);
        HttpRequest request = parseRequest(buffer);
        HttpResponse response;

        if (request.action == "GET") {
            response = requestGET(request);
        }
        else {
            response.status_line = "HTTP/1.1 404 Not Found";
        }

        std::string response_message = response.status_line + "\r\n";
        for (const auto& pair : response.headers)
        {
            response_message += pair.first + ": " + pair.second + "\r\n";
        }
        response_message += "\r\n" + response.body;

        // Attempt to send data to the connected socket.
        // client_socket is the file descriptor of a connected socket.
        // confirmation_buffer is a pointer to the data to be sent.
        // sizeof(confirmation_buffer) is the length (in bytes) of the data being sent.
        // Zero is the value to represent the use of the default flags.
        byte_count = send(client_socket, response_message.c_str(), response_message.size(), 0);
        if (byte_count < 0)
        {
            std::cerr << "Failed to send response to client" << std::endl;
        }
    }

    // Remove the closing socket from the active connections vector.
    size_t index = m_accepted_connections.size();
    for (size_t i = 0; i < m_accepted_connections.size(); ++i)
    {
        if (m_accepted_connections.at(i) == client_socket)
        {
            index = i;
        }
}
    m_accepted_connections.erase(m_accepted_connections.begin() + index);

    close(client_socket);

#elif _WIN32

    while (true)
    {
        // Attempt to receive data from the connected socket.
        // client_socket is the socket descriptor of a connected socket.
        // receive_buffer is a pointer to the data received.
        // sizeof(receive_buffer) is the length (in bytes) of the data received.
        // Zero is the value to represent the flag to use default behavior.
        int byte_count = recv(client_socket, receive_buffer, sizeof(receive_buffer), 0);
        if (byte_count < 0)
        {
            if (WSAGetLastError() == WSAECONNRESET)
            {
                std::cerr << "Client " << client_socket << " connection lost. Forcibly closed by client." << std::endl;
                break;
            }
            else
            {
                std::cout << "Server receive error: " << WSAGetLastError() << std::endl;
                WSACleanup();
                throw std::runtime_error("Server receive error!");
            }
        }
        else
        {
            std::cout << "Received data: " << receive_buffer << std::endl;
        }

        std::string buffer(receive_buffer);
        HttpRequest request = parseRequest(buffer);
        HttpResponse response;

        if (request.action == "GET") {
            response = requestGET(request);
        }
        else {
            response.status_line = "HTTP/1.1 404 Not Found";
        }

        std::string response_message = response.status_line + "\r\n";
        for (const auto& pair : response.headers)
        {
            response_message += pair.first + ": " + pair.second + "\r\n";
        }
        response_message += "\r\n" + response.body;

        // Attempt to send data to the connected socket.
        // client_socket is the socket descriptor of a connected socket.
        // confirmation_buffer is a pointer to the data to be sent.
        // sizeof(confirmation_buffer) is the length (in bytes) of the data being sent.
        // Zero is the value to represent the use of the default flags.
        byte_count = send(client_socket, response_message.c_str(), response_message.size(), 0);
        if (byte_count == SOCKET_ERROR)
        {
            std::cout << "Server send error: " << WSAGetLastError() << std::endl;
            WSACleanup();
            throw std::runtime_error("Server send error!");
        }
        else
        {
            std::cout << "Response sent to Client" << client_socket << ":\n " << response_message << std::endl;
        }
    }

    // Remove the closing socket from the active connections vector.
    size_t index = m_accepted_connections.size();
    for (size_t i = 0; i < m_accepted_connections.size(); ++i)
    {
        if (m_accepted_connections.at(i) == client_socket)
        {
            index = i;
        }
    }
    m_accepted_connections.erase(m_accepted_connections.begin() + index);

    // Close the client_socket to free up the system resources used.
    closesocket(client_socket);
    std::cout << "Accept socket closed." << std::endl;
#endif
}