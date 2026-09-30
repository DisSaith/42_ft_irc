/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofelten <nofelten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 14:15:14 by nofelten          #+#    #+#             */
/*   Updated: 2026/09/30 14:38:09 by nofelten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

/* ======================== Constructors / Destructor ======================== */

//Destructor
Client::~Client()
{
	if (DEBUG)
		std::cout << "Destructor called" << std::endl;
}

//Default Constructor
Client::Client() : 
	_fd(-1),
	_hasSetPass(false),
	_hasSetNick(false),
	_hasSetUser(false),
	_isOpperator(false),
	_toDisconnect(false)
{
	if (DEBUG)
		std::cout << "Default constructor called" << std::endl;
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

//Copy Constructor
Client::Client(const Client& copy)
{
	*this = copy;
	if (DEBUG)
		std::cout << "Copy constructor called" << std::endl;
}

//Overload operator=
Client&	Client::operator=(const Client& copy)
{
	if (DEBUG)
		std::cout << "Copy assignment operator called" << std::endl;
	if (this != &copy)
	{
		this->_fd = copy._fd;
		this->_ipAddr = copy._ipAddr;
		this->_bufferIn = copy._bufferIn;
		this->_bufferOut = copy._bufferOut;
		this->_nickname = copy._nickname;
		this->_username = copy._username;
		this->_realname = copy._realname;
		this->_hasSetPass = copy._hasSetPass;
		this->_hasSetNick = copy._hasSetNick;
		this->_hasSetUser = copy._hasSetUser;
		this->_isOpperator = copy._isOpperator;
		this->_toDisconnect = copy._toDisconnect;
	}
	return *this;
}

/* ======================== Getters / Setters ======================== */

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

	if (DEBUG)
		std::cout << "command -> " << command;

	return command;
}
