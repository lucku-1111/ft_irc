#ifndef MSGS_HPP
#define MSGS_HPP

#include "Client.hpp"
#include "Channel.hpp"
#include "Server.hpp"


void send_fd(int fd, std::string str);

const static std::string serverName = "ft_irc";
const static std::string colonServerName = ":localhost ";
const static std::string version = "1.0.0";

///// Welcome messages /////
// The first message sent after client registration
#define RPL_001_WELCOME(client) (colonServerName + "001 " + client + " :Welcome to the Internet Relay Network " + client + "\r\n")

// Part of the post-registration greeting
#define RPL_002_YOURHOST(client) (colonServerName + "002 " + client + " :Your host is " + serverName + ", running version " + version + "\r\n")

// Part of the post-registration greeting
#define RPL_003_CREATED(client) (colonServerName + "003 " + client + " :This server was created 2024 FEB 19 \r\n")

// Part of the post-registration greeting
#define RPL_004_SERVERINFO(client) (colonServerName + "004 " + client + " " + serverName + version + "- itkol " + "\r\n")


///// Error messages /////
// Response to not enough parameters
#define RPL_461_NEEDMOREPARAMS(client, cmd) (colonServerName + "461 " + client + " " + cmd + " :Not enough parameters\r\n")

// Response to already registered client
#define RPL_462_ALREADYREGISTRED(client, cmd) (colonServerName + "462 " + client +" " + cmd + " :Unauthorized command (already registered)\r\n")


#define RPL_JOIN(nick, hostname, servername, channel) ":" + nick + "!" + hostname + "@" + servername + " JOIN " + channel + "\r\n"


// Response to AWAY
#define RPL_301_AWAY(client, nick, message) (colonServerName + "301 " + client + " " + nick + ":" + message + "\r\n")

// Response to TOPIC when no topic is set
#define RPL_331_NOTOPIC(client, channel) (colonServerName + "331 " + client + " " + channel + " :No topic is set\r\n")

// Response to TOPIC when the topic is set
#define RPL_332_TOPIC(client, channel, topic) (colonServerName + "332 " + client + " " + channel + " :" + topic + "\r\n")

#define RPL_353_NAMREPLY(client, channel, clients) (colonServerName + " 353 " + client + " = " + channel + " :" + clients + "\r\n")

#define RPL_366_ENDOFNAMES(client, channel) (colonServerName + " 366 " + client + " " + channel + " :End of /NAMES list\r\n")


// Pong message
#define RPL_PONG(client) (colonServerName + "PONG " + serverName + " " + colonServerName)

#define RPL_QUIT(client) (colonServerName + client + " QUIT " + serverName + "\r\n")

#define RPL_JOIN(nick, hostname, servername, channel) ":" + nick + "!" + hostname + "@" + servername + " JOIN " + channel + "\r\n"

#endif
