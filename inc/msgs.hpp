#ifndef MSGS_HPP
#define MSGS_HPP

#include "Client.hpp"

#define serverName "ft_irc"
#define colonServerName ":localhost "

// The first message sent after client registration
#define RPL_001_WELCOME(client) (colonServerName + "001 " + client + " :Welcome to the Internet Relay Network " + client + "\r\n")

// Part of the post-registration greeting
#define RPL_002_YOURHOST(client) (colonServerName + "002 " + client + " :Your host is " + serverName + ", running version " + version + "\r\n")

// Part of the post-registration greeting
#define RPL_003_CREATED(client) (colonServerName + "003 " + client + " :This server was created 2024 FEB 19 \r\n")

// Part of the post-registration greeting
#define RPL_004_SERVERINFO(client) (colonServerName + "004 " + client + " " + serverName + version + "- itkol " + "\r\n")

// Response to AWAY
#define RPL_301_AWAY(client, nick, message) (colonServerName + "301 " + client + " " + nick + ":" + message + "\r\n")

// Response to TOPIC when no topic is set
#define RPL_331_NOTOPIC(client, channel) (colonServerName + "331 " + client + " " + channel + " :No topic is set\r\n")

// Response to TOPIC when the topic is set
#define RPL_332_TOPIC(client, channel, topic) (colonServerName + "332 " + client + " " + channel + " :" + topic + "\r\n")




#endif
