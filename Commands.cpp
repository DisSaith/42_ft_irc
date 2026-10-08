void	IrcServer::PASS(int const& fd)
{
	if (_recv.empty() == false && _recv.front() == "PASS")
	{
		//ERR_NEEDMOREPARAMS
		if (_recv.size() < 2)
			throw IrcException(461, "PASS", ":Not enough parameters", fd);
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
			//ERR_NEEDMOREPARAMS
			if (_recv.size() < 2)
				throw IrcException(461, "NICK", ":Not enough parameters", fd);
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
	if (_clients[fd]->setIsRegistered() == false)
		throw IrcException(-1, "JOIN", ":User not registered", fd);
	//ERR_NEEDMOREPARAMS
	if (_recv.size() < 2)
		throw IrcException(461, "JOIN", ":Not enough parameters", fd);
	std::string message;
	std::list<std::string>::iterator l_it = _recv.begin();
	std::list<std::string> channelNames = split(*(++l_it), ',');
	
	std::list<std::string> channelPasswords;
	if (_recv.size() == 3)
		channelPasswords = split(*(++l_it), ',');
	std::list<std::string>::iterator pass_it = channelPasswords.begin();

	for (std::list<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); it++)
	{
		try
		{
			std::map<std::string, Channel*>::iterator chan_it = _channels.find(*it);
			
			if ((*it).empty() == false && isMaskChar((*it)[0]) == false)
				throw IrcException(476, *it, ":Bad Channel Mask", fd);
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
					throw IrcException(-1, *it, ":User already on channel", fd);
				//ERR_INVITEONLYCHAN
				if (_channels[*it]->getIsInviteOnly())
					throw IrcException(473, *it, ":Cannot join channel (+i)", fd);
				//ERR_BADCHANNELKEY: missing key
				if (pass_it == channelPasswords.end() && _channels[*it]->getHasChannelKey())
					throw IrcException(475, *it, ":Cannot join channel (+k)", fd);
				//ERR_BADCHANNELKEY
				if (pass_it != channelPasswords.end() && _channels[*it]->getHasChannelKey() && (*pass_it).compare(_channels[*it]->getChannelKey()) != 0)
					throw IrcException(475, *it, ":Cannot join channel (+k)", fd);
				if (chan_it->second->getMembers().size() >= chan_it->second->getUserLimit())
					throw IrcException(471, *it, ":Cannot join channel (+l)", fd);

				chan_it->second->addNewMember(_clients[fd]);
				message = _clients[fd]->getNickname() + " has joined the channel " + *it + ".\n";
				chan_it->second->sendMessageToMembers(message, fd);
				message = "You joined the channel " + *it + ".\n";
				send(fd, message.c_str(), message.length(), 0);
   		        _channels[*it]->displayTopic(fd);
   	        	_channels[*it]->displayMembers(fd, false);
			}
		}
		catch ( IrcException const& e )
		{
			std::string message = buildMessage(e.getCode(), e.getTarget(), e.getText(), e.getFd());
			send(e.getFd(), message.c_str(), message.length(), 0);
		}
		if (pass_it != channelPasswords.end())
			pass_it++;
	}
}

void	IrcServer::PART(int const& fd)
{
	if (_clients[fd]->setIsRegistered() == false)
		throw IrcException(-1, "PART", ":User not registered", fd);	
	//ERR_NEEDMOREPARAMS
	if (_recv.size() < 2)
		throw IrcException(461, "PART", ":Not enough parameters", fd);

	std::list<std::string>::iterator l_it = _recv.begin();
	std::string channelName = *(++l_it);

	//ERR_NOTONCHANNEL
	if (_clients[fd]->getChannels().find(channelName) == _clients[fd]->getChannels().end())
		throw IrcException(461, "PART", ":Not enough parameters", fd);

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

void	IrcServer::KICK(int const& fd)
{
	if (_clients[fd]->setIsRegistered() == false)
		throw IrcException(-1, "KICK", ":User not registered", fd);	
	//ERR_NEEDMOREPARAMS
	if (_recv.size() < 2)
		throw IrcException(461, "KICK", ":Not enough parameters", fd);
	//ERR_CHANOPRIVSNEEDED
/*	if (!isAnyOperator(fd, chanTarget))
		throw IrcException(482, "KICK", ":You're not channel operator", fd);
*/
}

void	IrcServer::NAMES(int const& fd)
{
	bool notAll;

	if (_clients[fd]->setIsRegistered() == false)
		throw IrcException(-1, "NAMES", ":User not registered", fd);	
	std::list<std::string> channelNames;
	if (_recv.size() == 1)
	{
		notAll = false;
		for (std::map<std::string, Channel*>::iterator mapIt = _channels.begin(); mapIt != _channels.end(); mapIt++)
			channelNames.push_back(mapIt->first);
	}
	else
	{
		notAll = true;
		std::list<std::string>::iterator l_it = _recv.begin();
		channelNames = split(*(++l_it), ',');
	}

	for (std::list<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); it++)
	{
		std::map<std::string, Channel*>::iterator mapIt = _channels.find(*it);
		if (mapIt == _channels.end())
		{
			std::string errorMsg = *it + " :End of /NAMES list\n";
			send(fd, errorMsg.c_str(), errorMsg.length(), 0);
		}
		else
			_channels[*it]->displayMembers(fd, notAll);
	}
	if (notAll == false)
	{
		std::string errorMsg = " :End of /NAMES list\n";
		send(fd, errorMsg.c_str(), errorMsg.length(), 0);
	}
}

void	IrcServer::USER(int const& fd)
{
	if (_clients[fd]->getHasSetPass())
	{
		if (_recv.front() == "USER")
		{
			//ERR_NEEDMOREPARAMS
			if (_recv.size() < 5)
				throw IrcException(461, "USER", ":Not enough parameters", fd);
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
			std::string chanTarget = *it;
			it++; 

			std::string	message;
			while (it != _recv.end())
			{
				message += *it;
				it++;
				if (it != _recv.end())
					message += " ";
			}

			std::string fullMsg = ":" + _clients[fd]->getNickname() + " PRIVMSG " + chanTarget + " " + message + "\r\n";

			if (chanTarget[0] == '#' || chanTarget[0] == '&')
			{
				std::map<std::string, Channel*>::iterator chanIt = _channels.find(chanTarget);

				if (chanIt != _channels.end())
				{
					chanIt->second->sendMessageToMembers(fullMsg, fd);
				}
				else
				{
					std::string errorMsg = ":localhost 401 " + _clients[fd]->getNickname() + " " + chanTarget + " :No such nick/channel\r\n";
					send(fd, errorMsg.c_str(), errorMsg.length(), 0);
				}
				return ;
			}

			bool targetFound = false;
			int targetFd = -1;
			std::map<int, Client*>::iterator mapIt;

			for (mapIt = _clients.begin(); mapIt != _clients.end(); ++mapIt)
			{
				if (mapIt->second->getNickname() == chanTarget)
				{
					targetFound = true;
					targetFd = mapIt->first;
					break ;
				}
			}
			if (targetFound == false)
			{
				std::string errorMsg = ":localhost 401 " + _clients[fd]->getNickname() + " " + chanTarget + " :No such nick/channel\r\n";
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

void	IrcServer::MODE(int const& fd)
{
	if (_clients[fd]->setIsRegistered() == false)
		throw IrcException(-1, "MODE", ":User not registered", fd);
	//ERR_NEEDMOREPARAMS
	if (_recv.size() < 2)
		throw IrcException(461, "MODE", ":Not enough parameters", fd);
	std::list<std::string>::iterator it = _recv.begin();
	it++;
	std::string chanTarget = *it;
	if (chanTarget[0] == '#' || chanTarget[0] == '&')
	{
		std::map<std::string, Channel*>::iterator chanIt = _channels.find(chanTarget);
		if (chanIt == _channels.end())
		{
			std::string errorMsg = ":localhost 403 " + _clients[fd]->getNickname() + " " + chanTarget + " :No such channel\r\n";
			send(fd, errorMsg.c_str(), errorMsg.length(), 0);
			return ;
		}
		Channel* channel = chanIt->second;
		if (_recv.size() == 2)
		{
			std::string currentModes = "+";
			if (channel->getHasChannelKey())
				currentModes += "k";
			std::string modeMsg = ":localhost 324 " + _clients[fd]->getNickname() + " " + chanTarget + " " + currentModes + "\r\n";
			send(fd, modeMsg.c_str(), modeMsg.length(), 0);
			return ;
		}
		if (!isAnyOperator(fd, chanTarget))
		{
			std::string errorMsg = ":localhost 482 " + _clients[fd]->getNickname() + " " + chanTarget + " :You're not channel operator\r\n";
			send(fd, errorMsg.c_str(), errorMsg.length(), 0);
			return ;
		}
		it++;
		std::string flags = *it;
		it++;
		bool adding = true;
		std::string messageMode;
		std::string messageArgs;
		for (size_t i = 0; i < flags.length(); ++i)
		{
			char c = flags[i];
			if (c == '+')
			{
				adding = true;
				messageMode += "+";
			}
			else if (c == '-')
			{
				adding = false;
				messageMode += "-";
			}
			else if (c == 'o')
			{
				if (it != _recv.end())
				{
					std::string targetNick = *it;
					it++;	
					int targetFd = -1;
					std::map<int, Client*>::iterator clientIt;
					for (clientIt = _clients.begin(); clientIt != _clients.end(); ++clientIt)
					{
						if (clientIt->second->getNickname() == targetNick)
						{
							targetFd = clientIt->first;
							break ;
						}
					}
					if (targetFd != -1 && channel->hasMember(targetFd))
					{
						if (adding)
							channel->addNewOperator(_clients[targetFd]);
						else
							channel->removeOperator(targetFd);
						messageMode += "o";
						messageArgs += " " + targetNick;
					}
				}
			}
			else if (c == 'k')
			{
				if (adding)
				{
					if (it != _recv.end())
					{
						std::string newKey = *it;
						it++;
						channel->setHasChannelKey(true);
						channel->setChannelKey(newKey);
						messageMode += "k";
						messageArgs += " " + newKey;
					}
				}
				else
				{
					if (it != _recv.end())
					{
						std::string providedKey = *it;
						it++;
						if (providedKey == channel->getChannelKey())
						{
							channel->setHasChannelKey(false);
							channel->setChannelKey("");

							messageMode += "k";
							messageArgs += " " + providedKey;
						}
					}
				}
			}
			else if (c == 'l')
			{
				if (adding)
				{
					if (it != _recv.end())
					{
						unsigned int	limit;
						std::stringstream ss(*it);
						ss >> limit;
						channel->setUserLimit(limit);
						messageMode += "l";
						messageArgs += " " + *it;
						it++;
					}
				}
				else
				{
					if (it != _recv.end())
						channel->setUserLimit(UINT_MAX);
				}
			}
			else
			{
				std::string errorMsg = ":localhost 472 " + _clients[fd]->getNickname() + " " + c + " :is unknown mode char to me\r\n";
				send(fd, errorMsg.c_str(), errorMsg.length(), 0);
			}
		}
		if (messageMode.empty() == false && messageMode != "+" && messageMode != "-")
		{
			std::string fullMsg = ":" + _clients[fd]->getNickname() + " MODE " + chanTarget + " " + messageMode + messageArgs + "\r\n";
			channel->sendMessageToMembers(fullMsg, -1);
		}
	}
}
