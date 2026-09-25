/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofelten <nofelten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 14:15:14 by nofelten          #+#    #+#             */
/*   Updated: 2026/09/25 15:09:59 by nofelten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(): _fd(-1),
	_hasSetPass(false),
	_hasSetNick(false),
	_hasSetUser(false),
	_isOpperator(false),
	_toDisconnect(false)
{
	std::cout << "Default constructor called" << std::endl;
}

Client::Client(int fd): _fd(fd),
	_hasSetPass(false),
	_hasSetNick(false),
	_hasSetUser(false),
	_isOpperator(false),
	_toDisconnect(false)
{
	_bufferIn.reserve(1024);
	_bufferOut.reserve(1024);

	std::cout << "Fd construtor called" << std::endl;
}

Client::~Client()
{
	std::cout << "Destructor called" << std::endl;
}

Client::Client(const Client& copy)
{
	*this = copy;
	std::cout << "Copy constructor called" << std::endl;
}

Client&	Client::operator=(const Client& copy)
{
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

void Client::appendToIn(std::string const& data)
{
	this->_bufferIn += data;
}

bool Client::hasCompleteCommand() const
{
	return (this->_bufferIn.find("\r\n") != std::string::npos);
}

std::string Client::extractCommand()
{
	size_t pos = this->_bufferIn.find("\r\n");
	if (pos == std::string::npos)
		return "";

	std::string command = this->_bufferIn.substr(0, pos);

	this->_bufferIn.erase(0, pos + 2);

	return command;
}
