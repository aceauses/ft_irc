#include "Server.hpp"
#include "Channel.hpp"
#include "defines.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <ctime>
#include <iostream>
#include <ostream>
#include <poll.h>
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
	if (_server_socket >= 0) {
		close(_server_socket);
	}
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
	struct sockaddr_in _server_addr;
	_server_addr.sin_family      = AF_INET;
	_server_addr.sin_addr.s_addr = INADDR_ANY;
	_server_addr.sin_port        = htons(_port);
	if (bind(_server_socket, (struct sockaddr*)(&_server_addr),
			 sizeof(_server_addr))
		< 0) {
		std::cerr << "Error binding socket" << std::endl;
		return;
	}
	if (listen(_server_socket, 3) < 0) {
		std::cerr << "Error listening" << std::endl;
	}
	_server_ip            = inet_ntoa(_server_addr.sin_addr);
	struct pollfd newPool = {};
	newPool.fd            = _server_socket;
	newPool.revents       = 0;
	newPool.events        = POLLIN;
	_pollfds.push_back(newPool);
}

void Server::acceptConnection(int listeningSocket) {
	sockaddr_in clientAddr;
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

	std::cout << "New connection accepted" << std::endl;
}

std::string get_cmd(std::string data) {
	size_t pos = data.find_first_of(" ");
	if (pos != std::string::npos) {
		std::string cmd = data.std::string::substr(0, pos);
		// std::cout << "Command: " << cmd << std::endl;
		return cmd;
	}
	return "";
}

// need more checks for invalid input
std::string get_after_cmd(std::string data) {
	size_t pos = data.find_first_of(" ");
	if (pos != std::string::npos) {
		std::string after_cmd
			= data.std::string::substr(pos + 1);
		std::cout << "After Command: '" << after_cmd << "'"
				  << std::endl;
		return after_cmd;
	}
	return "";
}

void Server::executeCommand(Client const&      client,
							std::string const& data) {
	// std::cout << "Executing command: " << data << std::endl;
	if (get_cmd(data) == "JOIN") {
		std::string after = get_after_cmd(data);
		if (!after.empty() && after[0] == '#') {
			std::string channel_name = after.substr(
				1, after.find_first_of("\r\n") - 1);
			if (Server::findname(channel_name, _channels)
				== _channels.end()) {
				Channel newchannel(channel_name);
				_channels.push_back(newchannel);
				// std::cout << "Channel created: '" << channel_name
				// 		  << "'" << std::endl;
			}
			std::vector<Channel>::iterator join_channel
				= Server::findname(channel_name, _channels);
			if (join_channel != _channels.end()) {
				join_channel->addUser(client);
				client.Output(":" + client._nickname + "!"
							  + client._name + "@" + client._ip
							  + " JOIN :" + channel_name
							  + "\r\n");
				std::string to_join
					= ":" + client._nickname + "!" + client._name
					+ "@" + client._ip + " JOIN :" + channel_name
					+ "\r\n";
				join_channel->Message(client, to_join);
			} else {
				std::cerr << "Channel not found" << std::endl;
			}
		}
	} else if (data == "QUIT\r\n" || get_cmd(data) == "QUIT") {

		// the quit\r\n will quit without any message, get_cmd will also reaceive a message
		/* 
		
		 Example:

   		QUIT :Gone to have lunch        ; Preferred message format.

   		:syrk!kalt@millennium.stealth.net QUIT :Gone to have lunch ; User
                                   syrk has quit IRC to have lunch.
		
		
		 */
		close(client._ClientSocket);
	}
	if (get_cmd(data) == "PRIVMSG") {
		std::string after = get_after_cmd(data);
		if (!after.empty()) {
			std::string channel_or_user_name
				= after.substr(0, after.find_first_of(" "));
			std::string message
				= after.substr(after.find_first_of(" ") + 2);
			std::vector<Channel>::iterator dest_channel
				= Server::findname(channel_or_user_name,
								   _channels);
			if (dest_channel != _channels.end()) {
				dest_channel->Message(
					client,
					PRIVMSG(client._nickname, client._name,
							client._ip, dest_channel->_name,
							message));
			} else {
				std::vector<Client>::iterator dest_client
					= Server::findnickname(channel_or_user_name,
										   _clients);
				if (dest_client != _clients.end()) {
					dest_client->Output(PRIVMSG(
						client._nickname, client._name,
						client._ip, dest_client->_nickname,
						message));
				} else {
					// @todo send error message to client
					std::cerr << "Destination not found"
							  << std::endl;
				}
			}
		}
	}
}

void Server::pingClients() {
	time_t current_time = time(NULL);
	for (size_t i = 0; i < _clients.size(); i++) {
		// If it's time to ping this client
		if (current_time - _clients[i]._last_ping_sent
			> PING_INTERVAL) {
			std::string ping_message
				= PING(_clients[i]._nickname);
			if (send(_clients[i]._ClientSocket,
					 ping_message.c_str(), ping_message.size(),
					 0)
				< 0) {
				std::cout << "[ERROR] Sending ping to "
						  << _clients[i]._nickname << std::endl;
			} else {
				if (!_clients[i]._awaiting_pong) {
					_clients[i]._last_ping_sent = current_time;
					_clients[i]._awaiting_pong  = true;
					std::cout << "Sent PING to "
							  << _clients[i]._nickname
							  << std::endl;
				}
			}
		}

		// Check if client has timed out
		if (_clients[i]._awaiting_pong
			&& current_time - _clients[i]._last_ping_sent
				   > PING_INTERVAL) {
			std::cout << "Client " << _clients[i]._nickname
					  << " timed out" << std::endl;
			// Disconnect this client
			i--;
		}
	}
}

// static void remove_termination(std::string& data) {
// 	size_t pos = data.find_first_of("\r\n");
// 	if (pos != std::string::npos) {
// 		data = data.std::string::substr(0, pos);
// 	}
// }

bool Server::handleClientData(Client& client) {
	char    buffer[1024];
	ssize_t bytesReceived
		= recv(client._ClientSocket, buffer, sizeof(buffer), 0);
	if (bytesReceived == -1) {
		std::cerr << "Recv error: " << strerror(errno)
				  << std::endl;
		return false;
	} else if (bytesReceived == 0) {
		close(client._ClientSocket);
		return true;
	} else {
		client._inputBuffer.append(buffer, bytesReceived);
		processClientBuffer(client);
		return false;
	}
}

void Server::processClientBuffer(Client& client) {
	size_t pos;
	while ((pos = client._inputBuffer.find("\r\n"))
		   != std::string::npos) {
		std::string message = client._inputBuffer.substr(0, pos);
		client._inputBuffer.erase(0, pos + 2);

		std::cout << "[DEBUG] " << message << std::endl;

		if (!client._isConnected) {
			handleInitialConnection(client, message);
		} else {
			if (message.find("PONG") == 0) {
				client._awaiting_pong = false;
				std::cout << "Received PONG from "
						  << client._nickname << std::endl;
			} else {
				executeCommand(client, message);
			}
		}
	}
}

void Server::handleInitialConnection(
	Client& client, const std::string& message) {
	std::cout << "[DEBUG] Client init: " << message << std::endl;
	if (message.substr(0, 4) == "NICK") {
		client._nickname = message.substr(5);
		client.Output(WELCOME_MESSAGE);
	} else if (message.substr(0, 4) == "USER") {
		client._name = message.substr(message.find(':') + 1);
		client._isConnected = true;
	}
}
/*
@follow-up Function which starts the server and will wait for connections
@remind The user should be able to choose on which port and ip address to start the server
@note We need to handle signals so when we CTRL + C the server will stop in a clean way
*/
void Server::start() {
	makeSocket();
	std::cout << "Server started on " << _server_ip << ":"
			  << _port << std::endl;
	std::cout << "Waiting for connections..." << std::endl;
	// @follow-up Make the while loop to be until a signal was received
	while (_running) {
		// @follow-up Handle poll() and acceptConnection()
		if (poll(_pollfds.data(), _pollfds.size(), 0) == -1
			&& errno != EINTR) {
			std::cerr << "Poll error: " << strerror(errno)
					  << std::endl;
		}
		for (size_t i = 0; i < _pollfds.size(); ++i) {
			if (_pollfds[i].revents & POLLIN) {
				if (_pollfds[i].fd == _server_socket) {
					acceptConnection(_pollfds[i].fd);
				} else {
					if (handleClientData(_clients[i - 1])) {
						_clients.erase(_clients.begin()
									   + (i - 1));
						_pollfds.erase(_pollfds.begin() + i);
						std::cout << "[DEBUG] A Client "
									 "Disconnected "
								  << "Size of clients: "
								  << _clients.size()
								  << std::endl;
					}
				}
			}
		}
		pingClients();
	}
}