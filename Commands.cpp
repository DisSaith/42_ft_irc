void	IrcServer::PASS(int const& fd)
{
	if (_recv.empty() == false && _recv.front() == "PASS")
	{
		if (_recv.size() < 2)
		{
			std::string errorMsg = ":localhost 461 * PASS :Not enough parameters\r\n";
			send(fd, errorMsg.c_str(), errorMsg.length(), 0);
			return ;
		}
		std::list<std::string>::iterator it = _recv.begin();
		it++;
		if (*it != _password)
		{
			std::string errorMsg = ":localhost 464 * :Password incorrect\r\n";
			send(fd, errorMsg.c_str(), errorMsg.length(), 0);
			return ;
		}
		else if ( DEBUG )
			std::cout << "PASS \"" << *it << "\" " << GREEN << "CORRECT\r\n" << RESET;
		_clients[fd]->setPass();
	}
}

void	IrcServer::NICK(int const& fd)
{
	if (_clients[fd]->getHasSetPass() == true)
	{
		if (_recv.empty() == false && _recv.front() == "NICK")
		{
			if (_recv.size() < 2)
			{
				std::string errorMsg = ":localhost 461 * PASS :Not enough parameters\r\n";
				send(fd, errorMsg.c_str(), errorMsg.length(), 0);
				return ;
			}
			std::list<std::string>::iterator it = _recv.begin();
			it++;
			std::map<int, Client*>::iterator mapIt;
			for (mapIt = _clients.begin(); mapIt != _clients.end(); ++mapIt)
			{
				if (*it == mapIt->second->getNickname())
				{
					std::string errorMsg = ":localhost 433 * " + *it + " :Nickname is already in use\r\n";
					send(fd, errorMsg.c_str(), errorMsg.length(), 0);
					return ;
				}
			}
			_clients[fd]->setNickName(*it);
		}
	}
}

//Wrong error messages
void	IrcServer::JOIN(int const& fd)
{
	if (_recv.size() < 2)
	{
		std::string errorMsg = ":localhost 461 * JOIN :Not enough parameters\r\n";
		send(fd, errorMsg.c_str(), errorMsg.length(), 0);
		return ;
	}
	if (_recv.size() > 3)
	{
		std::string errorMsg = ":localhost 461 * JOIN :Too many parameters\r\n";
		send(fd, errorMsg.c_str(), errorMsg.length(), 0);
		return ;
	}
	if (_clients[fd]->setIsRegistered() == false)
	{
		std::string errorMsg = "JOIN : User not registered\r\n";
		send(fd, errorMsg.c_str(), errorMsg.length(), 0);
		return ;		
	}

	std::string message;
	std::list<std::string>::iterator l_it = _recv.begin();
	std::list<std::string> channelNames = split(*(++l_it), ',');
	if (_recv.size() == 3)
		std::list<std::string> channelPasswords = split(*(++l_it), ',');

	for (std::list<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); it++)
	{
		std::map<std::string, Channel*>::iterator chan_it = _channels.find(*it);
		if (chan_it == _channels.end())
		{
			_channels[*it] = new Channel(*it, _clients[fd]);
			_clients[fd]->setNewChannel(*it, _channels[*it]);
			message = _clients[fd]->getNickname() + " has created the channel " + *it + ".\n";
			send(fd, message.c_str(), message.length(), 0);
		}
		else
		{
			if (_channels[*it]->hasMember(fd))
			{
				message = "You already are in the channel " + *it + ".\n";
				send(fd, message.c_str(), message.length(), 0);
				continue ;
			}
			chan_it->second->addNewMember(_clients[fd]);
			message = _clients[fd]->getNickname() + " has joined the channel " + *it + ".\n";
			chan_it->second->sendMessageToMembers(message, fd);
			message = "You joined the channel " + *it + ".\n";
			send(fd, message.c_str(), message.length(), 0);
		}
	}
}

//if the owner leave, deletes the channel
void	IrcServer::PART(int const& fd)
{
	if (_recv.size() < 2)
	{
		std::string errorMsg = ":localhost 461 * PART :Not enough parameters\r\n";
		send(fd, errorMsg.c_str(), errorMsg.length(), 0);
		return ;
	}
	if (_recv.size() > 2)
	{
		std::string errorMsg = ":localhost 461 * PART :Too many parameters\r\n";
		send(fd, errorMsg.c_str(), errorMsg.length(), 0);
		return ;
	}
	if (_clients[fd]->setIsRegistered() == false)
	{
		std::string errorMsg = "PART : User not registered\r\n";
		send(fd, errorMsg.c_str(), errorMsg.length(), 0);
		return ;		
	}

	std::list<std::string>::iterator l_it = _recv.begin();
	std::string channelName = *(++l_it);

	if (_clients[fd]->getChannels().find(channelName) == _clients[fd]->getChannels().end())
	{
		std::string errorMsg = "You need to join this channel first\r\n";
		send(fd, errorMsg.c_str(), errorMsg.length(), 0);
		return ;
	}
	std::string message = _clients[fd]->getNickname() + " leaved the channel " + channelName + ".\n";
	_channels[channelName]->sendMessageToMembers( message, fd );
	message = "You leaved the channel " + channelName + ".\n";
	send(fd, message.c_str(), message.length(), 0);
	if (_channels[channelName]->removeMember(fd))
	{
		delete _channels[channelName];
		_channels.erase(channelName);
	}
}

void	IrcServer::NAMES(int const& fd)
{
	if (_recv.size() > 2)
	{
		std::string errorMsg = ":localhost 461 * NAMES :Too many parameters\r\n";
		send(fd, errorMsg.c_str(), errorMsg.length(), 0);
		return ;
	}
	if (_clients[fd]->setIsRegistered() == false)
	{
		std::string errorMsg = "NAMES : User not registered\r\n";
		send(fd, errorMsg.c_str(), errorMsg.length(), 0);
		return ;
	}
	std::list<std::string> channelNames;
	if (_recv.size() == 1)
	{
		for (std::map<std::string, Channel*>::iterator mapIt = _channels.begin(); mapIt != _channels.end(); mapIt++)
			channelNames.push_back(mapIt->first);
	}
	else
	{
		std::list<std::string>::iterator l_it = _recv.begin();
		channelNames = split(*(++l_it), ',');
	}

	for (std::list<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); it++)
	{
		std::map<std::string, Channel*>::iterator mapIt = _channels.find(*it);
		if (mapIt == _channels.end())
		{
			std::string errorMsg = "NAMES : No channel found with this name: " + *it + "\r\n";
			send(fd, errorMsg.c_str(), errorMsg.length(), 0);
		}
		else
			_channels[*it]->displayMembers(fd);
	}
}

void	IrcServer::USER(int const& fd)
{
	if (_clients[fd]->getHasSetPass())
	{
		if (_recv.front() == "USER")
		{
			if (_recv.size() < 5)
			{
				std::string errorMsg = ":localhost 461 * USER :Not enough parameters\r\n";
				send(fd, errorMsg.c_str(), errorMsg.length(), 0);
				return ;	
			}
			std::list<std::string>::iterator it = _recv.begin();
			it++;
			_clients[fd]->setUserName(*it);
			std::advance(it, 3);
			_clients[fd]->setRealName(*it);
			if (_clients[fd]->setIsRegistered())
			{
				std::string welcome = ":localhost 001 " + _clients[fd]->getNickname() + " :Welcome to the ft_irc network!\r\n";
				send(fd, welcome.c_str(), welcome.length(), 0);
				std::cout << "Le client " << fd << " est maintenant officiellement enregistré !" << std::endl;
			} 
		}
	}
}

void	IrcServer::PRIVMSG(int const& fd)
{
	if (_clients[fd]->setIsRegistered())
	{
		if (_recv.front() == "PRIVMSG")
		{
			if (_recv.size() == 1)
			{
				std::string errorMsg = ":localhost 411 " + _clients[fd]->getNickname() + " :No recipient given (PRIVMSG)\r\n";
				send(fd, errorMsg.c_str(), errorMsg.length(), 0);
				return ;
			}
			if (_recv.size() == 2)
			{
				std::string errorMsg = ":localhost 412 " + _clients[fd]->getNickname() + " :No text to send\r\n";
				send(fd, errorMsg.c_str(), errorMsg.length(), 0);
				return ;
			}
			
			std::list<std::string>::iterator it = _recv.begin();
			it++;
			std::string target = *it;
			it++; 
			
			std::string	message;
			while (it != _recv.end())
			{
				message += *it;
				it++;
				if (it != _recv.end())
					message += " ";
			}
			
			std::string fullMsg = ":" + _clients[fd]->getNickname() + " PRIVMSG " + target + " " + message + "\r\n";

			if (target[0] == '#' || target[0] == '&')
			{
				std::map<std::string, Channel*>::iterator chanIt = _channels.find(target);
				
				if (chanIt != _channels.end())
				{
					chanIt->second->sendMessageToMembers(fullMsg, fd);
				}
				else
				{
					std::string errorMsg = ":localhost 401 " + _clients[fd]->getNickname() + " " + target + " :No such nick/channel\r\n";
					send(fd, errorMsg.c_str(), errorMsg.length(), 0);
				}
				return ;
			}

			bool targetFound = false;
			int targetFd = -1;
			std::map<int, Client*>::iterator mapIt;

			for (mapIt = _clients.begin(); mapIt != _clients.end(); ++mapIt)
			{
				if (mapIt->second->getNickname() == target)
				{
					targetFound = true;
					targetFd = mapIt->first;
					break ;
				}
			}
			if (targetFound == false)
			{
				std::string errorMsg = ":localhost 401 " + _clients[fd]->getNickname() + " " + target + " :No such nick/channel\r\n";
				send(fd, errorMsg.c_str(), errorMsg.length(), 0);
				return ;
			}
			send(targetFd, fullMsg.c_str(), fullMsg.length(), 0);
		}
	}
}

void    IrcServer::QUIT(int const& fd)
{
	if (_recv.front() == "QUIT")
	{
		std::string quitMsg = "Client Quit";
		if (_recv.size() > 1)
		{
			std::list<std::string>::iterator it = _recv.begin();
			it++;
			quitMsg = "";
			while (it != _recv.end())
			{
				quitMsg += *it;
				it++;
				if (it != _recv.end())
					quitMsg += " ";
			}
			if (!quitMsg.empty() && quitMsg[0] == ':')
				quitMsg.erase(0, 1);
		}
		std::string fullMsg = ":" + _clients[fd]->getNickname() + " QUIT :" + quitMsg + "\r\n";
		std::set<int> clientsToNotify;
		std::map<std::string, Channel*>::iterator chanIt;
		for (chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt)
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
				chanIt->second->removeMember(fd);
			}
		}
		std::set<int>::iterator setIt;
		for (setIt = clientsToNotify.begin(); setIt != clientsToNotify.end(); ++setIt)
		{
			send(*setIt, fullMsg.c_str(), fullMsg.length(), 0);
		}
		std::cout << YELLOW << "Client " << _clients[fd]->getNickname() << " a quitté le serveur." << RESET << std::endl;
		_clients[fd]->setIsDisconnect(true);
	}
}