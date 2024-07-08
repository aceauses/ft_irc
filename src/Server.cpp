#include "Server.hpp"
#include "Channel.hpp"
#include "Client.hpp"
#include "debug.hpp"
#include "defines.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <iostream>
#include <ostream>
#include <poll.h>
#include <sstream>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define PORT 8989
#define PING_INTERVAL 60

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server()
	: _running(true)
	, _server_socket(-1)
	, _port(PORT)
	, _server_ip("not set") {
	instance = this;
}

Server::Server(const Server& src)
	: _running()
	, _server_socket()
	, _port(PORT)
	, _server_ip("not set") {
	*this = src;
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server() {
	if (_server_socket >= 0) { close(_server_socket); }
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

Server& Server::operator=(Server const& rhs) {
	(void)rhs;
	return *this;
}

/*
@note Function which creates a socket and binds it to the server address
*/
void Server::makeSocket() {
	_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (_server_socket < 0) {
		std::cerr << "Error creating socket" << std::endl;
		return;
	}
	int opt = 1;
	if (setsockopt(_server_socket, SOL_SOCKET,
				   SO_REUSEADDR | SO_REUSEPORT, &opt,
				   sizeof(opt))
		< 0) {
		std::cerr << "Error setting socket options" << std::endl;
		return;
	}
	struct sockaddr_in _server_addr = {.sin_family = AF_INET,
									   .sin_port = htons(_port),
									   .sin_addr = {
										   .s_addr = INADDR_ANY,
									   }};
	if (bind(_server_socket, (struct sockaddr*)(&_server_addr),
			 sizeof(_server_addr))
		< 0) {
		std::cerr << "Error binding socket" << std::endl;
		return;
	}
	if (listen(_server_socket, 3) < 0) {
		std::cerr << "Error listening" << std::endl;
	}
	_server_ip = inet_ntoa(_server_addr.sin_addr);
	_pollfds.push_back(
		(struct pollfd){_server_socket, POLLIN, 0});
}

void Server::acceptConnection(int listeningSocket) {
	sockaddr_in clientAddr    = {};
	socklen_t   clientAddrLen = sizeof(clientAddr);
	int         clientSocket  = accept(
        listeningSocket, (sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocket == -1) {
		std::cerr << "Accept error: " << strerror(errno)
				  << std::endl;
		return;
	}

	pollfd newPollFd = {clientSocket, POLLIN, 0};
	_pollfds.push_back(newPollFd);

	// Make a client and add it to the vector
	std::string ip = inet_ntoa(clientAddr.sin_addr);
	Client      newClient(ip, clientSocket);
	_clients.push_back(newClient);

	debug(SUCCESS, "New Connection Accepted [" + ip + "]");
}

void Server::pingClients() {
	time_t current_time = time(NULL);
	for (size_t i = 0; i < _clients.size(); i++) {
		if (current_time - _clients[i]._last_ping_sent
				> PING_INTERVAL
			&& !_clients[i]._awaiting_pong) {
			std::string ping_message
				= PING(_clients[i]._nickname);
			_clients[i].Output(ping_message);
			_clients[i]._last_ping_sent = current_time;
			_clients[i]._awaiting_pong  = true;
			debug(PING, "Sent to " + _clients[i]._nickname);
		}

		// Check if client has timed out
		if (_clients[i]._awaiting_pong
			&& current_time - _clients[i]._last_ping_sent
				   > PING_INTERVAL) {
			debug(TIMEOUT, "User " + _clients[i]._nickname);
			this->quit("", _clients[i]);
			i--;
		}
	}
}

void Server::handleClientData(Client& client) {
	char    buffer[1024];
	ssize_t bytesReceived
		= recv(client._ClientSocket, buffer, sizeof(buffer), 0);
	if (bytesReceived == -1) {
		std::cerr << "Recv error: " << strerror(errno)
				  << std::endl;
	} else if (bytesReceived == 0) {
		this->quit("", client);
	}
	client._inputBuffer.append(buffer, bytesReceived);
	processClientBuffer(client);
}

void Server::processClientBuffer(Client& client) {
	size_t pos;
	while ((pos = client._inputBuffer.find("\r\n"))
		   != std::string::npos) {
		std::string message = client._inputBuffer.substr(0, pos);
		client._inputBuffer.erase(0, pos + 2);

		if (!client._isConnected) {
			handleInitialConnection(client, message);
		} else {
			if (message.find("PONG") == 0) {
				client._awaiting_pong = false;
				debug(PONG, "Received from " + client._nickname);
			} else {
				executeCommand(client, message);
			}
		}
	}
}

bool Server::checkIfAlreadyConnected(Client& client) {
	debug(DEBUG,
		  "Check if there is a already connection with the data "
			  + client._nickname);
	for (size_t i = 0; i < _clients.size(); i++) {
		if (_clients[i]._nickname == client._nickname
			&& _clients[i]._ClientSocket
				   != client._ClientSocket) {
			client.Output(":127.0.0.1 433 * " + client._nickname
						  + " :Nickname "
							"is already in use\r\n");
			debug(WARNING, "Username already in use");
			return true;
		}
	}
	return false;
}

void Server::handleInitialConnection(
	Client& client, const std::string& message) {
	debug(DEBUG, "Init: " + message);
	if (message.substr(0, 4) == "NICK") {
		client._nickname = message.substr(5);
		if (!client._name.empty()
			&& !checkIfAlreadyConnected(client)) {
			client._isConnected = true;
			client.Output(WELCOME_MESSAGE);
		}
	} else if (message.substr(0, 4) == "USER") {
		client._name = message.substr(message.find(':') + 1);
		if (checkIfAlreadyConnected(client)) return;
		client.Output(WELCOME_MESSAGE);
		client._isConnected = true;
	}
}

std::string toString(int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}
/*
@remind The user should be able to choose on which port and ip address to start the server
@note We need to handle signals so when we CTRL + C the server will stop in a clean way
*/
void Server::start() {
	makeSocket();
	debug(INFO, "Server started on " + _server_ip + ":"
					+ toString(_port));
	debug(INFO, "Waiting for connections...");
	// @follow-up Make the while loop to be until a signal was received
	while (_running) {
		// @follow-up Handle poll() and acceptConnection()
		if (poll(_pollfds.data(), _pollfds.size(), 0) == -1
			&& errno != EINTR) {
			debug(ERROR, strerror(errno));
		}
		for (size_t i = 0; i < _pollfds.size(); i++) {
			if (_pollfds[i].revents & POLLIN) {
				if (_pollfds[i].fd == _server_socket) {
					acceptConnection(_pollfds[i].fd);
				} else {
					handleClientData(_clients[i - 1]);
				}
			}
		}
		pingClients();
	}
}