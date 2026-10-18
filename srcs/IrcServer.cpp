#include "../includes/IrcServer.hpp"
#include "Client.cpp"
#include "Channel.cpp"
#include "commands/Commands.cpp"

/* ======================== Orthodox Canonical Form (PRIVATES) ======================== */

IrcServer::IrcServer( IrcServer const& copy ) { (void)copy; }
IrcServer& IrcServer::operator=( IrcServer const& copy ) { (void)copy; return *this; }

/* ======================== Default Constructor & Destructor ======================== */

IrcServer::~IrcServer() 
{
	if (_serverStarted)
		std::cout << std::endl << BLUE << "IRC Server closed !" << RESET << std::endl;
}

IrcServer::IrcServer() : _password(""), _socketServer(-1), _port(0), _signalHandled(false), _serverStarted(false) {}

/* ======================== IRC Server ======================== */

void	IrcServer::InitServer( char* const& port, char* const& password )
{
	std::string string_port = std::string(port);

	if ( CheckServerPort(string_port) == true )
	{
		this->_port = ::stoi(string_port);
		this->_password = std::string(password);
	}
	else
	{
		std::cout << RED << "Error: " << RESET
			<< "IRC Server port not valid." << std::endl;
		return ;
	}
}

void IrcServer::CreateServer()
{
	// Config Server
	_server.sin_addr.s_addr = INADDR_ANY;
	_server.sin_family = AF_INET;
	_server.sin_port = htons(_port);

	// Creation of the initial Socket Server
	_socketServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socketServer == -1)
		throw std::runtime_error( "Failed during the creation of the server socket." );
	if (bind(_socketServer, (struct sockaddr*)&_server, sizeof(_server)) == -1)
		throw std::runtime_error( "Failed to bind the server socket." );
	if (listen(_socketServer, SOMAXCONN) == -1)
		throw std::runtime_error( "Failed of the listen() function." );
	fcntl(_socketServer, F_SETFL, O_NONBLOCK);

	InitMapCommands();

	std::cout << GREEN << "IRC Server created !\n" << RESET
		<< WHITE << "port: " << RESET << this->_port << std::endl
		<< WHITE << "password: " << RESET << this->_password << std::endl;

	this->_serverStarted = true;
}

void	IrcServer::ConnectionWithClients()
{
	struct pollfd	serverFd;
	serverFd.fd = _socketServer;
	serverFd.events = POLLIN;
	serverFd.revents = 0;
	_pollFds.push_back(serverFd);

	std::cout << BLUE << "Server listening." << RESET << " Waiting for connections..\n\n";

	while (true)
	{
		if (poll(&_pollFds[0], _pollFds.size(), -1) == -1)
		{
			if (!_signalHandled)
				std::cerr << "Critical error from poll()" << std::endl;
			break ;
		}
		for (size_t i = 0; i < _pollFds.size(); i++)
		{
			if (_pollFds[i].revents & POLLIN)
			{
				if (_pollFds[i].fd == _socketServer)
				{
					int newClientFd = accept(_socketServer, NULL, NULL);
					if (newClientFd == -1)
					{
						std::cerr << "Critical error from acept()" << std::endl;
						continue ;
					}
					fcntl(newClientFd, F_SETFL, O_NONBLOCK);
					_clients[newClientFd] = new Client(newClientFd);
					struct pollfd	clientFd;
					clientFd.fd = newClientFd;
					clientFd.events = POLLIN;
					clientFd.revents = 0;
					_pollFds.push_back(clientFd);				
				}
				else
				{
					char	buffer[4096];
					memset(buffer, 0, sizeof(buffer)); // cancel pb with zombie memomy

					int bytesRead = recv(_pollFds[i].fd, buffer, sizeof(buffer) - 1, 0);
					if (DEBUG)
					{
						std::cout << "\nbytesRead = " << bytesRead << std::endl
								<< "buffer -> ";
						for ( int i = 0 ; buffer[i] != '\0' ; i++ )
						{
							if ((int)buffer[i] < 32 || (int)buffer[i] > 126)
								std::cout << "[" << (int)buffer[i] << "] ";
							else 
								std::cout << buffer[i];
						}
						std::cout << std::endl;
					}
					if (bytesRead <= 0)
					{
						RemoveClient(_pollFds[i].fd);
						i--;
					}
					else
					{
						std::string data(buffer, bytesRead);
						_clients[_pollFds[i].fd]->appendToIn(data);
						while (_clients[_pollFds[i].fd]->hasCompleteCommand())
						{
							std::string cmd = _clients[_pollFds[i].fd]->extractCommand();
							if (DEBUG)
							{
								std::cout << YELLOW << "[Client " << _pollFds[i].fd << "]"
											<< RESET << " send : ";
								for ( int j = 0 ; cmd[j] != '\0' ; j++ )
								{
									if ((int)cmd[j] < 32 || (int)cmd[j] > 126)
										std::cout << "[" << (int)cmd[j] << "] ";
									else 
										std::cout << cmd[j];
								}
								std::cout << std::endl;
							}
							TokenizerRecv(cmd);
							try
							{
								ParsingRecv(_pollFds[i].fd);
							}
							catch ( IrcException const& e )
							{
								std::string message = buildMessage(e.getCode(), e.getTarget(), e.getText(), e.getFd());
								send(e.getFd(), message.c_str(), message.length(), 0);
							}
							if (_clients.find(_pollFds[i].fd) != _clients.end() && _clients[_pollFds[i].fd]->getToDisconnect() == true)
								break;
						}
						if (_clients.find(_pollFds[i].fd) != _clients.end() && _clients[_pollFds[i].fd]->getToDisconnect() == true)
						{
							RemoveClient(_pollFds[i].fd);
							i--;
						}
					}
				}	
			}
		}
	}
}


/* ======================== Handling Clients Messages ======================== */

// Ceussi ai la propriétée intellaictuel d'Entoine Coho
// Mairsi de ne plu i tousher.
// Caurdiallemant
void IrcServer::TokenizerRecv( std::string const& buffer )
{
	std::string		token;
	size_t			j = 0;
	bool			inWord = false;
	bool			colon = false;

	if (_recv.empty() == false)
		_recv.clear(); // clear list before every new recv from a client
	for ( size_t i = 0 ; i < buffer.length() ; i++)
	{
		if ( buffer[i] == ':' && i != 0 )
		{
			colon = true;
			inWord = true;
			j = i + 1;
		}
		if ( buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\r' && inWord == false )
		{
			inWord = true;
			j = i;
		}
		if ( ((buffer[i] == ' ' && !colon) || buffer[i] == '\n' || buffer[i] == '\r') && inWord == true )
		{
			inWord = false;
			token = buffer.substr(j, i - j);
			_recv.push_back(token);
			if (colon)
				colon = false;
		}
	}

	if (DEBUG) // display list tokens
	{
		int i = 0;
		std::cout << WHITE << "[List Tokens] " << RESET << std::endl;
		for ( std::list<std::string>::iterator it = _recv.begin() ; it != _recv.end() ; ++it )
		{
			std::cout << i << ": " << *it << std::endl;
			i++;
		}
		std::cout << std::endl;
	}
}

void IrcServer::ParsingRecv( int const& clientFd )
{
	std::map<std::string, cmdFunction>::iterator find;

	if (_recv.empty() == true)
		return ;
	if (_recv.front()[0] == ':')
	{
		_recv.pop_front(); // delete prefix if it exists
		if (_recv.empty() == true)
			return ;
	}
	find = _commands.find(_recv.front());
	if ( find != _commands.end() )
	{
		cmdFunction cmd = find->second;
		(this->*cmd)(clientFd);
	}
	else
	{
		if ( DEBUG )
			std::cerr << RED << "Command not found: " << RESET << _recv.front() << std::endl;

		//ERR_UNKNOWNCOMMAND
		std::string currentNick = _clients[clientFd]->getNickname().empty() ? "*" : _clients[clientFd]->getNickname();
		std::string errorMsg = ":localhost 421 " + currentNick + " " + _recv.front() + " :Unknown command\r\n";
		send(clientFd, errorMsg.c_str(), errorMsg.length(), 0);
	}
}

void IrcServer::RemoveClient( int fd )
{
	for (std::map<std::string, Channel*>::iterator chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt)
	{
		if (chanIt->second->hasMember(fd))
			chanIt->second->removeMember(fd);
	}

	for (size_t i = 0; i < _pollFds.size(); i++)
	{
		if (_pollFds[i].fd == fd)
		{
			_pollFds.erase(_pollFds.begin() + i);
			break;
		}
	}

	close(fd);
	if (_clients.find(fd) != _clients.end())
	{
		delete _clients[fd];
		_clients.erase(fd);
	}

	if (DEBUG)
		std::cout << WHITE << "All resources from client " << fd << " are free correctly." << RESET << std::endl;
}

/* ======================== Signals ======================== */

void	IrcServer::signalHandler( int signal )
{
	IrcServer *serverptr = GetPtrServer(NULL);

	serverptr->SetSignalHandled(true);

	if (DEBUG)
		std::cout << WHITE << "\nSignal " << RED << signal << WHITE << " intercepted\n\n" << RESET;
}


/* ======================== Utils ======================== */

// Get and save IrcServer pointer (for signals functions)
IrcServer* IrcServer::GetPtrServer( IrcServer *server )
{
	static IrcServer *serverPtr;

	if ( server != NULL )
		serverPtr = server;

	return serverPtr;
}

std::string		IrcServer::buildMessage( int const& code, std::string const& target, std::string const& text, int const& fd )
{
	std::string message = ":localhost ";

	if (code >= 0)
		message += itos(code) + " ";
	if (_clients[fd]->getNickname().empty())
		message += "* ";
	else
		message += _clients[fd]->getNickname() + " ";
	if (target.empty() == false)
		message += target + " ";
	message += text + "\r\n";

	return message;
}

//Search for the fd corresponding to a Nickname
int IrcServer::getFdfromNickname( std::string const& nickname )
{
	for (std::map<int, Client*>::iterator clients_it = _clients.begin(); clients_it != _clients.end(); clients_it++)
	{
		if (clients_it->second->getNickname() == nickname)
			return clients_it->first;
	}
	return -1;
}

bool IrcServer::isAnyOperator( int const& fd, std::string const& channelName )
{
	if (_clients[fd]->getIsServerOperator() || _channels[channelName]->getIsChannelOperator(fd))
		return true;
	return false;
}

// Ports between 0 and 1023 need root permission
// only ports between 1024 and 65 535 are allowed
bool IrcServer::CheckServerPort( std::string const& port ) const
{
	int converted_port;

	if ( port.empty() == true )
		throw std::invalid_argument("This IRC Server port is invalid.");
	if (port[0] == '-')
		throw std::invalid_argument("This IRC Server port is out of range.");

	for ( size_t i = 0 ; i < port.length() ; i++ )                            
	{  
		if (!std::isdigit(port[i]))
			throw std::invalid_argument("This IRC Server port is invalid.");
	}

	converted_port = ::stoi(port);
	if ( converted_port >= 0 && converted_port < 1024 )
		throw std::out_of_range( "This IRC Server port needs root permission." );
	else if ( converted_port < 0 || converted_port > 65535 )
		throw std::out_of_range( "This IRC Server port is out of range." );
	else if ( converted_port >= 1024 && converted_port <= 65535 )
		return true;
	return false;
}

// Initialize map container of IRC Server commands
void	IrcServer::InitMapCommands()
{
	if (_commands.empty() == false)
		_commands.clear();

	_commands["PASS"] = &IrcServer::PASS;
	_commands["NICK"] = &IrcServer::NICK;
	_commands["USER"] = &IrcServer::USER;
	_commands["JOIN"] = &IrcServer::JOIN;
	_commands["PRIVMSG"] = &IrcServer::PRIVMSG;
	_commands["PART"] = &IrcServer::PART;
	_commands["NAMES"] = &IrcServer::NAMES;
	_commands["QUIT"] = &IrcServer::QUIT;
	_commands["MODE"] = &IrcServer::MODE;
	_commands["KICK"] = &IrcServer::KICK;
	_commands["TOPIC"] = &IrcServer::TOPIC;
	_commands["INVITE"] = &IrcServer::INVITE;
	_commands["OPER"] = &IrcServer::OPER;
	_commands["CAP"] = &IrcServer::CAP;
	_commands["PING"] = &IrcServer::PING;
}

//Setter
void	IrcServer::SetSignalHandled( bool set )
{
	this->_signalHandled = set;
}

// Close all fds and delete for no leaks at the end of the program
void IrcServer::CloseFds()
{

	close(_socketServer);

	for ( size_t i = 0 ; i < _pollFds.size() ; i++ )
	{
		if (_pollFds[i].fd != _socketServer)
			close(_pollFds[i].fd);
		delete(_clients[_pollFds[i].fd]);
		_clients.erase(_pollFds[i].fd);
		_pollFds.erase(_pollFds.begin() + i);
		i--;
	}
}

void IrcServer::DeleteChannels()
{
	std::map<std::string, Channel*>::iterator it = _channels.begin();

	while ( it != _channels.end() )
	{
		delete(it->second);
		++it;
	}
}
