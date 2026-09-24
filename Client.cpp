/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nofelten <nofelten@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/23 14:15:14 by nofelten          #+#    #+#             */
/*   Updated: 2026/09/24 10:35:56 by nofelten         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client(): _fd(-1),
	_hasSetPass(false),
	_hasSetNick(false),
	_hasSetUser(false),
	_isRegistred(false),
	_isOpperator(false),
	_toDisconnect(false)
{
	std::cout << "Default constructor called" << std::endl;
}

Client::Client(int fd): _fd(fd),
	_hasSetPass(false),
	_hasSetNick(false),
	_hasSetUser(false),
	_isRegistred(false),
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
		this->_isRegistred = copy._isRegistred;
		this->_isOpperator = copy._isOpperator;
		this->_toDisconnect = copy._toDisconnect;
	}
	return *this;
}
