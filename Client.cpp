/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofelten <nofelten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 14:15:14 by nofelten          #+#    #+#             */
/*   Updated: 2026/10/02 12:58:29 by acohaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/* ======================== Orthodox Canonical Form (PRIVATES) ======================== */

Client::Client() {}
Client::Client(const Client& copy) { (void) copy; }
Client&	Client::operator=(const Client& copy) { (void) copy; return *this; }


/* ======================== Main Constructors & Destructor ======================== */

//Destructor
Client::~Client()
{
	if (DEBUG)
		std::cout << "Destructor called" << std::endl;
}

//Main Constructor
Client::Client(int fd) : 
	_fd(fd),
	_hasSetPass(false),
	_hasSetNick(false),
	_hasSetUser(false),
	_isOpperator(false),
	_toDisconnect(false)
{
	_bufferIn.reserve(1024);
	_bufferOut.reserve(1024);

	if (DEBUG)
		std::cout << "Fd construtor called" << std::endl;
}


/* ======================== Getters / Setters ======================== */

std::string Client::getNickname() const
{
	return this->_nickname;
}

std::string Client::getCurrentChannelName() const
{
	return this->_currentchannelname;
}

bool	Client::getHasSetPass() const
{
	return this->_hasSetPass;
}

int Client::getFd() const 
{
	return this->_fd;
}

void	Client::setPass()
{
	_hasSetPass = true;
}

void	Client::setNickName(std::string const& nickName)
{
	_nickname = nickName;
	_hasSetNick = true;
}

void	Client::setNewChannel(std::string const& channelName)
{
	_currentchannelname = channelName;
}

void	Client::setUserName(std::string const& userName)
{
	_username = userName;
	_hasSetUser = true;
}

void	Client::setRealName(std::string const& realName)
{
	_realname = realName;
}

bool	Client::setIsRegistered() const
{
	return (this->_hasSetPass && this->_hasSetNick && this->_hasSetUser);
}


/* ======================== Methods ======================== */

void Client::appendToIn(std::string const& data)
{
	this->_bufferIn += data;

	if (DEBUG)
		std::cout << "_bufferIn.length() = " << _bufferIn.length() << std::endl;

	if (_bufferIn.length() == 510)
		_bufferIn += "\r\n";
}

// Return true or false if there is a '\n' or not in the initial query by the user
// Some Clients and Servers don't respect the protocol RFC 1459 for IRC so we
// decided to accept the two cases (even if it doesn't respect the norm)
bool Client::hasCompleteCommand() const
{
	return (this->_bufferIn.find("\n") != std::string::npos);
	//return (this->_bufferIn.find("\r\n") != std::string::npos);
}

// Return the extracted command (without the \r and \n)
std::string Client::extractCommand()
{
	size_t pos = this->_bufferIn.find("\n");
	if (pos == std::string::npos)
		return "";

	std::string command = this->_bufferIn.substr(0, pos + 1);

	// Erase the extract command and the '\n' in buffer
	this->_bufferIn.erase(0, pos + 1);

	return command;
}
