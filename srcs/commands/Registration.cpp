
void	IrcServer::PASS(int const& fd)
{
	if (_recv.empty() == false && _recv.front() == "PASS")
	{
		if (_clients[fd]->getHasSetPass())
			throw IrcException(-1, "PASS", ":Password already set", fd);
		//ERR_ALREADYREGISTRED
		if (_clients[fd]->getIsRegistered())
			throw IrcException(462, "PASS", ":You may not reregister", fd);
		//ERR_NEEDMOREPARAMS
		if (_recv.size() < 2)
		{
			_clients[fd]->setIsDisconnect(true);
			throw IrcException(461, "PASS", ":Not enough parameters", fd);
		}
		std::list<std::string>::iterator it = _recv.begin();
		it++;
		//ERR_PASSWDMISMATCH
		if (*it != _password)
		{
			_clients[fd]->setIsDisconnect(true);
			throw IrcException(464, "PASS", ":Password incorrect", fd);
		}
		else if ( DEBUG )
			std::cout << GREEN <<"Correct Password: "<< RESET << *it
					<< YELLOW << " [client " << fd << "]\n" << RESET;
			
		_clients[fd]->setPass();
	}
}

bool	isValidNickname(const std::string& nick)
{
	if (nick.empty() || nick.length() > 9)
		return false;
	std::string special = "[]\\`_^{}|";
	if (!std::isalpha(nick[0]) && special.find(nick[0]) == std::string::npos)
		return false;
	for (size_t i = 1; i < nick.length(); ++i)
	{
		if (!std::isalnum(nick[i]) && special.find(nick[i]) == std::string::npos && nick[i] != '-')
			return false;
	}
	return true;
}

void	IrcServer::NICK(int const& fd)
{
	if (_recv.empty() == false && _recv.front() == "NICK")
	{
		if (_clients[fd]->getHasSetPass() == false)
			throw IrcException(451, "NICK", ":You have not registered", fd);
		//ERR_NONICKNAMEGIVEN
		if (_recv.size() < 2)
			throw IrcException(431, "NICK", ":No nickname given", fd);
		std::list<std::string>::iterator it = _recv.begin();
		it++;
		std::string newNick = *it;
		// ERR_ERRONEUSNICKNAME (432)
		if (!isValidNickname(newNick))
		{
			std::string target = "NICK " + newNick;
			throw IrcException(432, target, ":Erroneus nickname", fd);
		}
		//ERR_NICKNAMEINUSE (433)
		for (std::map<int, Client*>::iterator mapIt = _clients.begin(); mapIt != _clients.end(); ++mapIt)
		{
			if (mapIt->first != fd && newNick == mapIt->second->getNickname())
			{
				std::string target = "NICK " + newNick;
				throw IrcException(433, target, ":Nickname is already in use", fd);
			}
		}
		std::string oldNick = _clients[fd]->getNickname();
		_clients[fd]->setNickName(newNick);

		if (_clients[fd]->getIsRegistered() && oldNick.empty())
		{
			std::string welcome = ":localhost 001 " + _clients[fd]->getNickname() + " :Welcome to the ft_irc network!\r\n";
			send(fd, welcome.c_str(), welcome.length(), 0);
			std::cout << GREEN << "Client " << fd << " is officially registered !\n" << RESET;
		}

		if (!oldNick.empty())
		{
			std::string nickMsg = ":" + oldNick + " NICK " + newNick + "\r\n";
			send(fd, nickMsg.c_str(), nickMsg.length(), 0);
			std::set<int> clientsToNotify;
			for (std::map<std::string, Channel*>::iterator chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt)
			{
				if (chanIt->second->hasMember(fd))
				{
					std::map<int, Client*> members = chanIt->second->getMembers();
					std::map<int, Client*>::iterator memIt;
					for (memIt = members.begin(); memIt != members.end(); ++memIt)
					{
						if (memIt->first != fd)
							clientsToNotify.insert(memIt->first);
					}
				}
			}
			std::set<int>::iterator setIt;
			for (setIt = clientsToNotify.begin(); setIt != clientsToNotify.end(); ++setIt)
			{
				send(*setIt, nickMsg.c_str(), nickMsg.length(), 0);
			}
		}
	}
}

void	IrcServer::USER(int const& fd)
{
	if (_recv.front() == "USER")
	{
		if (_clients[fd]->getHasSetPass() == false)
			throw IrcException(451, "USER", ":You have not registered", fd);
		//ERR_ALREADYREGISTRED
		if (_clients[fd]->getIsRegistered())
			throw IrcException(462, "USER", ":You may not reregister", fd);
		//ERR_NEEDMOREPARAMS
		if (_recv.size() < 5)
			throw IrcException(461, "USER", ":Not enough parameters", fd);
		std::list<std::string>::iterator it = _recv.begin();
		it++;
		_clients[fd]->setUserName(*it);
		std::advance(it, 3);
		_clients[fd]->setRealName(*it);
		if (_clients[fd]->getIsRegistered())
		{
			std::string welcome = ":localhost 001 " + _clients[fd]->getNickname() + " :Welcome to the ft_irc network!\r\n";
			send(fd, welcome.c_str(), welcome.length(), 0);
			std::cout << GREEN << "Client " << fd << " is officially registered !\n" << RESET;
		} 
	}
}
