#include "Operator.hpp"

Operator::Operator( void ): Client()
{
    _isOperator = true;
}

Operator::Operator( int fd ): Client(fd)
{
    _isOperator = true;
}

Operator::Operator( Operator const & src ) : Client(src)
{
    _isOperator = true;
}

Operator::~Operator( void )
{}

//Overload operator=
Operator&	Operator::operator=(const Operator& copy)
{
	if (DEBUG)
		std::cout << "Operator copy assignment operator called" << std::endl;
	if (this != &copy)
	{
		_fd = copy._fd;
		_ipAddr = copy._ipAddr;
		_bufferIn = copy._bufferIn;
		_bufferOut = copy._bufferOut;
		_nickname = copy._nickname;
		_username = copy._username;
		_realname = copy._realname;
		_hasSetPass = copy._hasSetPass;
		_hasSetNick = copy._hasSetNick;
		_hasSetUser = copy._hasSetUser;
		_isOperator = copy._isOperator;
		_toDisconnect = copy._toDisconnect;
	}
	return *this;
}