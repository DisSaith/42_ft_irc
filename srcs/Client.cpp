#include "../includes/Client.hpp"

/* ======================== Orthodox Canonical Form (PRIVATES) ======================== */

Client::Client() {}
Client::Client( const Client& copy ) { (void) copy; }
Client&	Client::operator=( const Client& copy ) { (void) copy; return *this; }

/* ======================== Main Constructors & Destructor ======================== */

//Destructor
Client::~Client()
{
	std::cout << YELLOW << "Client " << _fd << RESET
				<< " left server." << std::endl;
}

//Main Constructor
Client::Client( int fd ) : 
	_fd(fd),
	_hasSetPass(false),
	_hasSetNick(false),
	_hasSetUser(false),
	_isOperator(false),
	_toDisconnect(false)
{
	_bufferIn.reserve(1024);
	_bufferOut.reserve(1024);

	std::cout << YELLOW << "Client " << fd << RESET
				<< " joined server." << std::endl;
}

//Parametric constructor, mainly used for debug
Client::Client( int fd, std::string const& nickname, std::string const& username, std::string const& realname ) : 
	_nickname(nickname),
	_username(username),
	_realname(realname),
	_fd(fd),
	_hasSetPass(true),
	_hasSetNick(true),
	_hasSetUser(true),
	_isOperator(false),
	_toDisconnect(false)
{
	_bufferIn.reserve(1024);
	_bufferOut.reserve(1024);

	if (DEBUG)

		std::cout << YELLOW << "Client " << fd << RESET
					<< " joinded server." << std::endl;
}

/* ======================== Getters / Setters ======================== */

std::string Client::getNickname() const
{
	return _nickname;
}

std::map<std::string, Channel*> Client::getChannels() const
{
	return _channels;
}

int	Client::getFd() const 
{
	return _fd;
}

bool	Client::getHasSetPass() const
{
	return _hasSetPass;
}

bool	Client::getIsServerOperator() const
{
	return _isOperator;
}

bool	Client::getToDisconnect() const
{
	return _toDisconnect;
}

bool    Client::getIsInvited( std::string const& channelName ) const
{
        return (std::find(_invitedChannels.begin(), _invitedChannels.end(), channelName) != _invitedChannels.end());
}

bool	Client::getIsRegistered() const
{
	return (this->_hasSetPass && this->_hasSetNick && this->_hasSetUser);
}

std::string	Client::getAllNames() const
{
	return _nickname + "!" + _username + "@" + _hostname;
}

void	Client::setPass()
{
	_hasSetPass = true;
}

void	Client::setNickName( std::string const& nickName )
{
	_nickname = nickName;
	_hasSetNick = true;
	if (DEBUG)
		displayClient();
}

void	Client::setNewChannel( std::string const& channelName, Channel *channel )
{
	_channels[channelName] = channel;
	if (DEBUG)
		displayClient();
}

void	Client::removeChannel( std::string const& channelName )
{
	std::map<std::string, Channel*>::iterator it = _channels.find(channelName);
	if (it != _channels.end())
		_channels.erase(channelName);
	if (DEBUG)
		displayClient();
}

void	Client::setUserName( std::string const& userName )
{
	_username = userName;
	_hasSetUser = true;
}

void	Client::setHostName( std::string const& hostName )
{
	_hostname = hostName;
}

void	Client::setRealName( std::string const& realName )
{
	_realname = realName;
}

void	Client::setIsDisconnect( bool status )
{
	_toDisconnect = status;
}

void Client::setIsOperator( bool status )
{
	_isOperator = status;
	if (DEBUG)
		displayClient();
}


/* ======================== Methods ======================== */

// Return the extracted command (without the \r and \n)
std::string Client::extractCommand()
{
	size_t pos = 0;

	if ( this->_bufferIn.find("\n") != std::string::npos 
			|| this->_bufferIn.find("\r") != std::string::npos)
	pos = FindEndFirstCommand();
	else
		return "";

	std::string command = this->_bufferIn.substr(0, pos + 1);

	// Erase the extract command and the '\n' in buffer
	this->_bufferIn.erase(0, pos + 1);

	return command;
}

size_t	Client::FindEndFirstCommand()
{
	size_t	pos1 = _bufferIn.find("\r");
	size_t	pos2 = _bufferIn.find("\n");
	size_t	pos = 0;

	if ( pos2 == pos1 + 1 )
		pos = pos2;
	else if ( pos1 < pos2 )
		pos = pos1;
	else
		pos = pos2;
	return pos;
}

// Return true or false if there is a '\n' or '\r' not in the initial query by the user
// Some Clients and Servers don't respect the protocol RFC 1459 for IRC so we
// decided to accept the two cases (even if it doesn't respect the norm)
bool Client::hasCompleteCommand() const
{
	if ( this->_bufferIn.find("\n") != std::string::npos 
			|| this->_bufferIn.find("\r") != std::string::npos)
		return true;
	return false;
}

void Client::appendToIn( std::string const& data )
{
	this->_bufferIn += data;

	if (_bufferIn.length() >= 510)
	{
		_bufferIn.erase(510, _bufferIn.length());
		_bufferIn += "\r\n";
	}

	if (DEBUG)
		std::cout << "_bufferIn.length() = " << _bufferIn.length() << std::endl;
}

void    Client::addInvite( std::string const& channelName )
{
        if (!getIsInvited(channelName))
                _invitedChannels.push_back(channelName); // On ajoute le nom au vecteur de strings
}

void    Client::removeInvite( std::string const& channelName )
{
        std::vector<std::string>::iterator it = std::find(_invitedChannels.begin(), _invitedChannels.end(), channelName);

        if (it != _invitedChannels.end())
                _invitedChannels.erase(it);
}

void Client::displayClient( void )
{
	try
	{
		std::string	message = "\n---CLIENT " + _nickname + "---\n";

		message += "USERNAME :" + _username + "\n";
		message += "REALNAME :" + _realname + "\n";
		message += "HOSTNAME :" + _hostname + "\n";
		message += "FULLNAME :" + this->getAllNames() + "\n";
		message += "FD       :" + ::itos(_fd) + "\n";
		message += "Registered  : " + ::itos(static_cast<int>(getIsRegistered())) + "\n";
		message += "Server Operator  : " + ::itos(static_cast<int>(_isOperator)) + "\n";
		message += "Channels : ";
		for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); it++)
			message += it->first + " ";
		message += "\nInvites : ";
		for (std::vector<std::string>::iterator it = _invitedChannels.begin(); it != _invitedChannels.end(); it++)
			message += *it + " ";	
		std::cout << message << std::endl << std::endl;
	}
	catch(std::exception const& e)
	{
		std::cerr << RED <<"Error while allocated client debug message" << RESET << std::endl;
		std::cerr << e.what() << std::endl;
	}
}
