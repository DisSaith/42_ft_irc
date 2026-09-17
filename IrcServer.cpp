/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IrcServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: acohaut <acohaut@learner.42.tech>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/17 11:03:07 by acohaut           #+#    #+#             */
/*   Updated: 2026/09/17 12:19:01 by acohaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "IrcServer.hpp"

/* ======================== Constructors / Destructor ======================== */

IrcServer::~IrcServer() {} //Destructor

//Constructors
IrcServer::IrcServer() : _port(0), _password(0) {}

IrcServer::IrcServer( int port, int password ) : _port(port), _password(password) 
{
	std::cout << GREEN << "IRC Server created !\n" << RESET
				<< "port: " << this->_port << std::endl
				<< "password: " << this->_password << std::endl;
}

IrcServer::IrcServer( std::string port, std::string password ) 
{
	this->_port = Utils::stoi(port);
	this->_password = Utils::stoi(password);

	std::cout << GREEN << "IRC Server created !\n" << RESET
				<< "port: " << this->_port << std::endl
				<< "password: " << this->_password << std::endl;
}

IrcServer::IrcServer( IrcServer const& copy )
{
	this->_port = copy._port;
	this->_password = copy._password;
}

//Overload operator=
IrcServer& IrcServer::operator=( IrcServer const& copy )
{
    if (this != &copy)
    {
		this->_port = copy._port;
		this->_password = copy._password;
    }
    return *this;
}

/* ======================== Methods ======================== */
