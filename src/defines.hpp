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

// topic reply
#define RPL_NOTOPIC                                             \
	":" + _server_ip + " 331 " + client._nickname + " "         \
		+ channel_name + " :No topic is set\r\n"

// channel limit reached
#define ERR_CHANNELISFULL                                       \
	":" + _server_ip + " 471 " + client._nickname + " "         \
		+ channel_name + " :Cannot join channel (+l)\r\n"

#define RPL_TOPIC                                               \
	":" + _server_ip + " 332 " + client._nickname + " "         \
		+ channel_name + " :" + channel->_topic + "\r\n"

#define ERR_CHANOPRIVSNEEDED                                    \
	":" + _server_ip + " 482 " + client._nickname + " "         \
		+ channel_name + " :You're not channel operator\r\n"
#define ERR_PASSWDMISMATCH                                      \
	":" + _server_ip + " 464 * :Password incorrect\r\n"

#define ERR_NEEDMOREPARAMS                                      \
	":" + _server_ip + " 461 * PASS :Not enough parameters\r\n"

// :originNick!originUser@originHost PART #irctest :I'm joined to too many channels already
#define PART_REPLY(client, current_channel_name)                \
	":" + client._nickname + "!" + client._name + "@"           \
		+ client._ip + " PART " + current_channel_name + "\r\n"

// "<channel name> :No such channel"
#define ERR_NOSUCHCHANNEL                                       \
	"403 " + client._nickname + " " + channel_name              \
		+ " :No such channel" + "\r\n"

// "<client name> <channel name> :You're not on that channel"
#define ERR_NOTONCHANNEL                                        \
	"442 " + client._nickname + " " + channel_name              \
		+ " :You're not on that channel" + "\r\n"

#define ERR_USERNOTINCHANNEL                                    \
	"441 " + client._nickname + " " + channel_name              \
		+ " :They aren't on that channel" + "\r\n"

#define KICK_NOTICE(comment)                                    \
	":" + client._nickname + "!" + client._name + "@"           \
		+ client._ip + " KICK " + channel_name + " "            \
		+ user->_name + " :" + comment + "\r\n"

#define ERR_NOSUCHNICK(invitee_nick)                            \
	":" + _server_ip + " 401 " + client._nickname + " "         \
		+ invitee_nick + " :No such nick/channel\r\n"

#define ERR_USERONCHANNEL(invitee_nick)                         \
	":" + _server_ip + " 443 " + client._nickname + " "         \
		+ invitee_nick + " " + channel->_name              \
		+ " :is already on channel\r\n"

#define INVITE                                                  \
	":" + client._nickname + " INVITE " + invitee->_nickname    \
		+ " " + channel->_name + "\r\n"

#define RPL_INVITING                                            \
	"341 " + client._nickname + " " + invitee->_nickname + " "  \
		+ channel->_name + "\r\n"