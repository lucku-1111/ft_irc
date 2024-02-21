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
// 001 : The first message sent after client registration
#define RPL_001_WELCOME(client) (colonServerName + "001 " + client + " :Welcome to the Internet Relay Network " + client + "\r\n")

// 002 : Part of the post-registration greeting
#define RPL_002_YOURHOST(client) (colonServerName + "002 " + client + " :Your host is " + serverName + ", running version " + version + "\r\n")

// 003 : Part of the post-registration greeting
#define RPL_003_CREATED(client) (colonServerName + "003 " + client + " :This server was created 2024 FEB 19 \r\n")

// 004 :Part of the post-registration greeting
#define RPL_004_SERVERINFO(client) (colonServerName + "004 " + client + " " + serverName + version + "- itkol " + "\r\n")


///// Error messages /////

///// Channel and User commands /////

// 331: No topic is set
#define RPL_331_NOTOPIC(channel) (colonServerName + "331 " + client + " " + channel + " :No topic is set\r\n")

// 332: Topic message
#define RPL_332_TOPIC(channel, topic) (colonServerName + "332 " + client + " " + channel + " :" + topic + "\r\n")

// 341: Reply to INVITE showing who was invited to which channel
#define RPL_341_INVITE(client, channel) (colonServerName + "341 " + client + " " + channel + "\r\n")

///// Names list /////

// 353: Reply to NAMES command
#define RPL_353_NAMREPLY(client, channel, names) (colonServerName + "353 " + client + " = " + channel + " :" + names + "\r\n")

// 366: End of NAMES list
#define RPL_366_ENDOFNAMES(client, channel) (colonServerName + "366 " + client + " " + channel + " :End of NAMES list\r\n")

///// Server messages /////

// 372: MOTD text line
#define RPL_372_MOTD(text) (colonServerName + "372 " + client + " :- " + text + "\r\n")

// 375: Start of MOTD command
#define RPL_375_MOTDSTART(text) (colonServerName + "375 " + client + " :- " + text + " Message of the day - \r\n")

// 376: End of MOTD command
#define RPL_376_ENDOFMOTD() (colonServerName + "376 " + client + " :End of MOTD command\r\n")

///// Error and Status messages /////

// 401: No such nick/channel
#define RPL_401_NOSUCHNICK(client, nick) (colonServerName + "401 " + client + " " + nick + " :No such nick/channel\r\n")

// 403: No such channel
#define RPL_403_NOSUCHCHANNEL(client, channel) (colonServerName + "403 " + client + " " + channel + " :No such channel\r\n")

// 431: No nickname given
#define RPL_431_NONICKNAMEGIVEN(client) (colonServerName + "431 " + client + " :No nickname given\r\n")

// 432: Erroneous nickname
#define RPL_432_ERR_ERRONEUSNICKNAME(client) (colonServerName + "432 " + client + " :Erroneous nickname\r\n")

// 433: Nickname is already in use
#define RPL_433_ERR_NICKNAMEINUSE(client) (colonServerName + "433 " + client + " :Nickname is already in use\r\n")

// 442: You're not on that channel
#define RPL_442_NOTONCHANNEL(channel) (colonServerName + "442 " + client + " " + channel + " :You're not on that channel\r\n")

// 461 : Not enough parameters
#define RPL_461_NEEDMOREPARAMS(client, cmd) (colonServerName + "461 " + client + " " + cmd + " :Not enough parameters\r\n")

// 462 : Unauthorized command (already registered)
#define RPL_462_ALREADYREGISTRED(client, cmd) (colonServerName + "462 " + client +" " + cmd + " :Unauthorized command (already registered)\r\n")

// 464 : Password incorrect
#define RPL_464_PASSWDMISMATCH() (colonServerName + "464 " + ":Password incorrect\r\n")

// 472: Unknown MODE flag
#define RPL_472_UNKNOWNMODE(char) (colonServerName + "472 " + client + " " + char + " :is unknown mode char to me\r\n")

// 473: Cannot join channel (+i)
#define RPL_473_INVITEONLYCHAN(channel) (colonServerName + "473 " + client + " " + channel + " :Cannot join channel (+i)\r\n")

// 475: Cannot join channel (+k)
#define RPL_475_BADCHANNELKEY(channel) (colonServerName + "475 " + client + " " + channel + " :Cannot join channel (+k)\r\n")

// 477: Channel does not support channel modes
#define RPL_477_NOCHANMODES(channel, mode) (colonServerName + "477 " + client + " " + channel + " : channel does not support "+ mode + "mode\r\n")

// 482: Not channel operator
#define RPL_482_CHANOPRIVSNEEDED(client, channel) (colonServerName + "482 " + client + " " + channel + " :You're not channel operator\r\n")

///// Command responses /////

// Response to NICK
#define RPL_NICK(nick, hostname, servername, newnick) ":" + nick + "!" + hostname + "@" + servername + " NICK :" + newnick + "\r\n"

// Response to JOIN
#define RPL_JOIN(nick, hostname, servername, channel) ":" + nick + "!" + hostname + "@" + servername + " JOIN :" + channel + "\r\n"

// Response to PART
#define RPL_PART(nick, hostname, servername, channel) ":" + nick + "!" + hostname + "@" + servername + " PART :" + channel + "\r\n"

// Response to pivate message
#define RPL_PRIVMSG(nick, user, servername, channel, msg) ":" + nick + "!" + user + "@" + servername + " PRIVMSG " + channel + " :" + msg + "\r\n"

// Pong message
#define RPL_PONG(client) (colonServerName + "PONG " + serverName + " " + colonServerName)

// Response to Quit
#define RPL_QUIT(client) (colonServerName + client + " QUIT: " + serverName + "\r\n")

#endif
