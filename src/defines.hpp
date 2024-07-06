#pragma once

/**
 * @brief acknowledgement of a user joining a channel
 */
#define JOINEDREPLY                                             \
	":" + client._nickname + "@"                                \
		+ client._ip /* client ip @note change */ + " JOIN "    \
		+ _name /* (prefix +)channel name */                    \
		+ "\r\n"

#define JOINED_NOTICE                                           \
	":" + client._nickname + "!" + client._name + "@"           \
		+ client._ip + " JOIN :"                                \
		+ _name /* (prefix +)channel name */ + "\r\n"

#define WELCOME_MESSAGE                                         \
	":" + _server_ip + " 001 " + client._nickname               \
		+ " :Welcome "                                          \
		  "to the IRC server!\r\n"

#define PING(client_name)                                       \
	("PING " + std::string(client_name) + "\r\n")

// :Angel!wings@irc.org PRIVMSG Wiz :Are you receiving this message ?// Wiz sends message to Angel
// Angel input is:
// PRIVMSG Wiz :Are you receiving this message ?

// :originNick!originUser@originHost PRIVMSG originNick :message // #originUser sends message to originNick
#define PRIVMSG_USER                                            \
	":" + client._nickname + "!" + client._name + "@"           \
		+ client._ip + " PRIVMSG " + dest_client->_nickname     \
		+ " :" + message + "\r\n"

// :originNick!originUser@originHost PRIVMSG #test :message // #originUser sends message to #test
#define PRIVMSG_CHANNEL                                         \
	":" + client._nickname + "!" + client._name + "@"           \
		+ client._ip + " PRIVMSG " + dest_channel->_name + " :" \
		+ message + "\r\n"

// :originNick!originUser@originHost PART #irctest :I'm joined to too many channels already
#define PART_REPLY(client, current_channel_name)                \
	(":" + (client)._nickname + "!" + (client)._name + "@"      \
	 + (client)._ip + " PART :" + (current_channel_name)        \
	 + "\r\n")
