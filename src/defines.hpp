#pragma once

#define MAX_INPUT_LEN 510

#define JOINEDREPLY                                             \
	":" + client._nickname + "@" + client._ip + " JOIN "        \
		+ _name + "\r\n"

#define JOINED_NOTICE                                           \
	":" + client._nickname + "!" + client._name + "@"           \
		+ client._ip + " JOIN :" + _name + "\r\n"

#define WELCOME_MESSAGE                                         \
	":" + _server_ip + " 001 " + client._nickname               \
		+ " :Welcome "                                          \
		  "to the IRC server!\r\n"

#define PING(client_name)                                       \
	("PING " + std::string(client_name) + "\r\n")

#define PRIVMSG_USER                                            \
	":" + client._nickname + "!" + client._name + "@"           \
		+ client._ip + " PRIVMSG " + dest_client->_nickname     \
		+ " :" + message + "\r\n"

#define PRIVMSG_CHANNEL                                         \
	":" + client._nickname + "!" + client._name + "@"           \
		+ client._ip + " PRIVMSG " + dest_channel->_name + " :" \
		+ message + "\r\n"

#define RPL_NOTOPIC                                             \
	":" + _server_ip + " 331 " + client._nickname + " "         \
		+ channel_name + " :No topic is set\r\n"

#define ERR_CHANNELISFULL                                       \
	":" + _server_ip + " 471 " + client._nickname + " "         \
		+ channel_name + " :Cannot join channel (+l)\r\n"

#define RPL_TOPIC(channel)                                      \
	":" + _server_ip + " 332 " + client._nickname + " "         \
		+ channel_name + " :" + channel->_topic + "\r\n"

#define ERR_CHANOPRIVSNEEDED                                    \
	":" + _server_ip + " 482 " + client._nickname + " "         \
		+ channel_name + " :You're not channel operator\r\n"

#define ERR_PASSWDMISMATCH                                      \
	":" + _server_ip + " 464 * :Password incorrect\r\n"

#define ERR_NEEDMOREPARAMS(cmd)                                 \
	":" + _server_ip + " 461 * " + cmd                          \
		+ " :Not enough parameters\r\n"

#define PART_REPLY(client, current_channel_name)                \
	":" + client._nickname + "!" + client._name + "@"           \
		+ client._ip + " PART " + current_channel_name + "\r\n"

#define ERR_NOSUCHCHANNEL(cmd)                                  \
	":" + _server_ip + " 403  * " + cmd + " :No such channel\r\n"

#define ERR_NOTONCHANNEL(cmd)                                   \
	":" + _server_ip + " 442  * " + cmd                         \
		+ " :You aren't on that channel\r\n"

#define ERR_USERNOTINCHANNEL(_server_ip, user_name)             \
	":" + _server_ip + " 441 " + user_name                      \
		+ " :They aren't on that channel" + "\r\n"

#define KICK_NOTICE                                             \
	":" + client._nickname + "!" + client._name + "@"           \
		+ client._ip + " KICK " + channel_name + " "            \
		+ user_name + " " + comment + "\r\n"

#define ERR_NOSUCHNICK(invitee_nick)                            \
	":" + _server_ip + " 401 " + invitee_nick                   \
		+ " :No such nick/channel\r\n"

#define ERR_USERONCHANNEL(invitee_nick)                         \
	":" + _server_ip + " 443 " + invitee_nick + " "             \
		+ channel->_name + " : is already on channel\r\n"

#define INVITE                                                  \
	":" + client._nickname + " INVITE " + invitee->_nickname    \
		+ " " + channel->_name + "\r\n"

#define RPL_INVITING                                            \
	"341 " + client._nickname + " " + invitee->_nickname + " "  \
		+ channel->_name + "\r\n"

#define ERR_INVITEONLYCHAN(name)                                \
	":" + _server_ip + " 473 * " + name                         \
		+ " : Cannot join channel (+i)\r\n"

#define ERR_ERRONEUSNICKNAME(client)                            \
	":" + _server_ip + " 432 " + client._nickname               \
		+ " :Erroneous nickname\r\n"

#define ERR_CHANNELNAMETOOLONG                                  \
	":" + _server_ip + " 403 " + client._nickname + " "         \
		+ channel_name + " :Channel name is too long\r\n"

#define ERR_BADCHANNELKEY                                       \
	":" + _server_ip + " 461 * JOIN"                            \
		+ " :Cannot join channel (+k)\r\n"

#define ERR_NONICKNAMEGIVEN                                     \
	":" + _server_ip + " 431 " + client._nickname               \
		+ " :Nickname not given\r\n"

#define ERR_NICKNAMEINUSE                                       \
	":" + _server_ip + " 433 " + client._nickname               \
		+ " :Nickname already in use\r\n"

#define NICK_REPLY                                              \
	":" + oldNick + "!" + client._name + "@" + client._ip       \
		+ " NICK " + after + "\r\n"

#define ERR_KEYSET                                              \
	":" + _server_ip + " 467 " + client._nickname               \
		+ " :Channel key already set\r\n"

#define ERR_UNKNOWNMODE(_char)                                  \
	":" + _server_ip + " 472 " + client._nickname + " " + _char \
		+ " :is unknown mode char to me for " + channel_name    \
		+ "\r\n"

#define ERR_NORECIPIENT(_char)                                  \
	":" + _server_ip + " 411 " _char + " :No recipient given "  \
		+ _char + "\r\n"

#define ERR_NOTEXTTOSEND                                        \
	":" + _server_ip + " 412 * :No text to send\r\n"

#define ERR_CANNOTSENDTOCHAN(channel_name)                      \
	":" + _server_ip + " 404 " + client._nickname + " "         \
		+ channel_name + " :Cannot send to channel\r\n"
