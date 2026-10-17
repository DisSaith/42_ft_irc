#include "Bot.hpp"

/* ----- Main Constructor & Destructor ----- */

Bot::Bot ( char **argv ): _ip(argv[1]),
					_port(Utils::stoi(argv[2])),
					_password(argv[3]), 
					_botName(argv[4]), 
					_maxNbWarnings(Utils::stoi(argv[5])), 
					_lastInviteNickname(""), 
					_sock(socket(AF_INET, SOCK_STREAM, 0))
{}

Bot::~Bot( void ){}


/* ----- Getters / Setters ----- */

std::string                 Bot::getIp() const
{
	return _ip;
}

int                         Bot::getPort() const
{
	return _port;
}

std::string                 Bot::getPassword() const
{
	return _password;
}

std::string                 Bot::getBotname() const
{
	return _botName;
}

int                         Bot::getMaxNbWarnings() const
{
	return _maxNbWarnings;
}

std::map<std::string, int>  Bot::getUsersWarnings() const
{
	return _usersWarnings;
}

std::string                 Bot::getLastInviteNickname() const
{
	return _lastInviteNickname;
}

int                         Bot::getSocket() const
{
	return _sock;
}

void 		                Bot::setLastInviteNickname(std::string const& name)
{
	_lastInviteNickname = name;
	_lastInviteNickname.erase(0, 1);
}

void                        Bot::incrementUserWarnings(std::string const& nickname)
{
	_usersWarnings[nickname]++;
}