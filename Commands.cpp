void	IrcServer::JOIN( int const& fd )
{
	if (_clients[fd]->getIsRegistered() == false)
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
				if (_channels[*it]->getIsInviteOnly() && _clients[fd]->getIsInvited(*it) == false)
					throw IrcException(473, *it, ":Cannot join channel (+i)", fd);
				//ERR_BADCHANNELKEY: missing key
				if (pass_it == channelPasswords.end() && _channels[*it]->getHasChannelKey())
					throw IrcException(475, *it, ":Cannot join channel (+k)", fd);
				//ERR_BADCHANNELKEY
				if (pass_it != channelPasswords.end() && _channels[*it]->getHasChannelKey() && (*pass_it).compare(_channels[*it]->getChannelKey()) != 0)
					throw IrcException(475, *it, ":Cannot join channel (+k)", fd);
				if (chan_it->second->getMembers().size() >= chan_it->second->getUserLimit())
					throw IrcException(471, *it, ":Cannot join channel (+l)", fd);

				if (_clients[fd]->getIsInvited(*it))
					_clients[fd]->removeInvite(*it);
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

void	IrcServer::PART( int const& fd )
{
	if (_clients[fd]->getIsRegistered() == false)
		throw IrcException(-1, "PART", ":User not registered", fd);	
	//ERR_NEEDMOREPARAMS
	if (_recv.size() < 2)
		throw IrcException(461, "PART", ":Not enough parameters", fd);

	std::list<std::string>::iterator l_it = _recv.begin();
	std::string channelName = *(++l_it);

	//ERR_NOTONCHANNEL
	if (_clients[fd]->getChannels().find(channelName) == _clients[fd]->getChannels().end())
		throw IrcException(461, "PART", ":You're not on that channel", fd);

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

void	IrcServer::KICK( int const& operator_fd )
{
	if (_clients[operator_fd]->getIsRegistered() == false)
		throw IrcException(-1, "KICK", ":User not registered", operator_fd);	
	//ERR_NEEDMOREPARAMS
	if (_recv.size() < 3)
		throw IrcException(461, "KICK", ":Not enough parameters", operator_fd);

	std::list<std::string>::iterator it = _recv.begin();
	std::string channelName = *(++it);
	std::string nickName = *(++it);
	std::string comment;
	++it;
	for (; it != _recv.end(); it++)
	{
		comment += *it;
		if (it != _recv.end())
			comment += " ";
	}
	int		kicked_fd = getFdfromNickname(nickName);
	if (kicked_fd == -1)
		throw IrcException(-1, "KICK", ":No such nickname", operator_fd);

	//ERR_CHANOPRIVSNEEDED
	if (!isAnyOperator(operator_fd, channelName))
		throw IrcException(482, "KICK", ":You're not channel operator", operator_fd);
	//ERR_NOTONCHANNEL
	if (_clients[kicked_fd]->getChannels().find(channelName) == _clients[kicked_fd]->getChannels().end())
		throw IrcException(461, "KICK", ":User not on that channel", operator_fd);
	//ERR_NOSUCHCHANNEL
	if (_channels.find(channelName) == _channels.end())
		throw IrcException(403, "KICK", ":No such channel", operator_fd);
	//ERR_NOSUCHNICK
	if (_clients.find(kicked_fd) == _clients.end())
		throw IrcException(401, "KICK", ":No such nick/channel", operator_fd);

	std::string message = _clients[kicked_fd]->getNickname() + " has been kicked from " + channelName + " because of :" + comment +  ".\n";
	_channels[channelName]->sendMessageToMembers( message, kicked_fd );

	if (_clients[operator_fd]->getChannels().find(channelName) == _clients[operator_fd]->getChannels().end())
		send(operator_fd, message.c_str(), message.length(), 0);

	message = "You have been kicked from " + channelName + " because of :" + comment +  ".\n";
	send(kicked_fd, message.c_str(), message.length(), 0);
	if (_channels[channelName]->removeMember(kicked_fd))
	{
		delete _channels[channelName];
		_channels.erase(channelName);
	}
}

void	IrcServer::NAMES( int const& fd )
{
	bool notAll;

	if (_clients[fd]->getIsRegistered() == false)
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
		_channels[*it]->displayMembers(fd, notAll);
	if (notAll == false)
	{
		std::string message;
		std::string name;

		message = "* :";
		for (std::map<int, Client*>::const_iterator it = _clients.begin(); it != _clients.end(); ++it)
		{
			if (it->second->getChannels().empty())
			{
				name = "";
				if (it->second->getIsServerOperator())
					name = name + "@";
				name = name + it->second->getNickname();
				message = message + name;
				if (++it != _clients.end())
					message = message + " ";
				--it;
			}
		}
		message = message + "\n";
		if (message.size() <= 4)
			message = "";
		message += ":End of /NAMES list\n";
		send(fd, message.c_str(), message.length(), 0);
	}
}

void	IrcServer::TOPIC( int const& fd )
{
	if (_clients[fd]->getIsRegistered() == false)
		throw IrcException(-1, "TOPIC", ":User not registered", fd);	
	//ERR_NEEDMOREPARAMS
	if (_recv.size() < 2)
		throw IrcException(461, "TOPIC", ":Not enough parameters", fd);

	std::string	message;
	std::list<std::string>::iterator recv_it = _recv.begin();
	std::string channelName = *(++recv_it);
	++recv_it;
	std::string topic;
	for (; recv_it != _recv.end(); recv_it++)
	{
		topic += *recv_it;
		if (recv_it != _recv.end())
			topic += " ";
	}

	//ERR_NOSUCHCHANNEL
	if (_channels.find(channelName) == _channels.end())
		throw IrcException(403, "TOPIC", ":No such channel", fd);

	if (_recv.size() == 2)
	{
		if (_channels[channelName]->getTopic().empty())
			topic = "No topic is set";
		else
			topic = _channels[channelName]->getTopic();
		message = channelName + " :" + topic + ".\n";
		send(fd, message.c_str(), message.length(), 0);
	}
	else
	{
		if (_channels[channelName]->getHasTopicRestrictions() == true)
			throw IrcException(-1, "TOPIC", ":channel has topic restrictions", fd);
		_channels[channelName]->setTopic(topic);
	}
}

void	IrcServer::PRIVMSG(int const& fd)
{
	//ERR_NOTREGISTERE
	if (_clients[fd]->getIsRegistered() == false)
		throw IrcException(451, "PRIVMSG", ":You have not registered", fd);
	if (_recv.front() == "PRIVMSG")
	{
		//ERR_NORECIPIENT
		if (_recv.size() == 1)
			throw IrcException(411, "", ":No recipient given (PRIVMSG)", fd);
		//ERR_NOTEXTTOSEND
		if (_recv.size() == 2)
			throw IrcException(412, "", ":No text to send", fd);
		std::list<std::string>::iterator it = _recv.begin();
		it++;
		std::string targetsString = *it;
		it++;
		std::string	message;
		while (it != _recv.end())
		{
			message += *it;
			it++;
			if (it != _recv.end())
				message += " ";
		}
		std::list<std::string> targetsList = split(targetsString, ',');
		//ERR_TOOMANYTARGETS
		if (targetsList.size() > 10)
			throw IrcException(407, targetsString, ":Duplicate recipients. No message delivered", fd);
		for (std::list<std::string>::iterator targetIt = targetsList.begin(); targetIt != targetsList.end(); ++targetIt)
		{
			try
			{
				std::string chanTarget = *targetIt;
				if (chanTarget.empty())
					continue;
				//ERR_NOTOPLEVEL
				if (chanTarget.find('*') != std::string::npos || chanTarget.find('?') != std::string::npos)
				{
					if (chanTarget.find('.') == std::string::npos)
						throw IrcException(413, chanTarget, ":No toplevel domain specified", fd);
					else
						throw IrcException(414, chanTarget, ":Wildcard in toplevel domain", fd);
				}
				std::string fullMsg = ":" + _clients[fd]->getNickname() + " PRIVMSG " + chanTarget + " :" + message + "\r\n";
				if (chanTarget[0] == '#' || chanTarget[0] == '&')
				{
					std::map<std::string, Channel*>::iterator chanIt = _channels.find(chanTarget);
					if (chanIt != _channels.end())
					{
						//ERR_CANNOTSENDTOCHA
						if (!chanIt->second->hasMember(fd))
							throw IrcException(404, chanTarget, ":Cannot send to channel", fd);
						chanIt->second->sendMessageToMembers(fullMsg, fd);
					}
					//ERR_NOSUCHNICK
					else
						throw IrcException(401, chanTarget, ":No such nick/channel", fd);
					continue ;
				}
				bool targetFound = false;
				int targetFd = -1;
				for (std::map<int, Client*>::iterator mapIt = _clients.begin(); mapIt != _clients.end(); ++mapIt)
				{
					if (mapIt->second->getNickname() == chanTarget)
					{
						targetFound = true;
						targetFd = mapIt->first;
						break ;
					}
				}
				//ERR_NOSUCHNICK
				if (targetFound == false)
					throw IrcException(401, chanTarget, ":No such nick/channel", fd);
				send(targetFd, fullMsg.c_str(), fullMsg.length(), 0);
			}
			catch ( IrcException const& e )
			{
				std::string errorMsg = buildMessage(e.getCode(), e.getTarget(), e.getText(), e.getFd());
				send(e.getFd(), errorMsg.c_str(), errorMsg.length(), 0);
			}
		}
	}
}

void    IrcServer::QUIT( int const& fd )
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
				if (chanIt->second->removeMember(fd))
				{
					delete chanIt->second;
					_channels.erase(chanIt->first);
				}
			}
		}
		std::set<int>::iterator setIt;
		for (setIt = clientsToNotify.begin(); setIt != clientsToNotify.end(); ++setIt)
		{
			send(*setIt, fullMsg.c_str(), fullMsg.length(), 0);
		}
		_clients[fd]->setIsDisconnect(true);
	}
}

void	IrcServer::MODE( int const& fd )
{
	//ERR_NOTREGISTERED
	if (_clients[fd]->getIsRegistered() == false)
		throw IrcException(451, "MODE", ":You have not registered", fd);
	//ERR_NEEDMOREPARAMS
	if (_recv.size() < 2)
		throw IrcException(461, "MODE", ":Not enough parameters", fd);
	std::list<std::string>::iterator it = _recv.begin();
	it++;
	std::string chanTarget = *it;
	if (chanTarget[0] != '#' && chanTarget[0] != '&')
	{
		//ERR_USERSDONTMATCH
		if (chanTarget != _clients[fd]->getNickname())
			throw IrcException(502, "MODE", ":Cannot change mode for other users", fd);
		return ;
	}
	std::map<std::string, Channel*>::iterator chanIt = _channels.find(chanTarget);
	//ERR_NOSUCHCHANNEL
	if (chanIt == _channels.end())
		throw IrcException(403, chanTarget, ":No such channel", fd);
	Channel* channel = chanIt->second;
	if (_recv.size() == 2)
	{
		std::string currentModes = "+";
		std::string currentArgs = "";
		if (channel->getIsInviteOnly())
			currentModes += "i";
		if (channel->getHasTopicRestrictions())
			currentModes += "t";
		if (channel->getHasChannelKey())
		{
			currentModes += "k";
			currentArgs += " " + channel->getChannelKey();
		}
		if (channel->getUserLimit() != UINT_MAX)
		{
			currentModes += "l";
			std::stringstream ss;
			ss << channel->getUserLimit();
			currentArgs += " " + ss.str();
		}
		if (currentModes == "+")
			currentModes = "";
		//RPL_CHANNELMODEIS
		std::string modeMsg = ":localhost 324 " + _clients[fd]->getNickname() + " " + chanTarget + " " + currentModes + currentArgs + "\r\n";
		send(fd, modeMsg.c_str(), modeMsg.length(), 0);
		return ;
	}
	//ERR_CHANOPRIVSNEEDED
	if (!isAnyOperator(fd, chanTarget))
		throw IrcException(482, chanTarget, ":You're not channel operator", fd);
	it++;
	std::string flags = *it;
	it++;
	bool adding = true;
	std::string messageMode;
	std::string messageArgs;
	for (size_t i = 0; i < flags.length(); ++i)
	{
		try
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
				//ERR_NEEDMOREPARAMS
				if (it == _recv.end())
					throw IrcException(461, "MODE", ":Not enough parameters", fd);
				std::string targetNick = *it;
				it++;
				int targetFd = getFdfromNickname(targetNick);
				//ERR_NOSUCHNICK
				if (targetFd == -1)
					throw IrcException(401, targetNick, ":No such nick/channel", fd);
				//ERR_USERNOTINCHANNEL
				if (!channel->hasMember(targetFd))
					throw IrcException(441, targetNick, chanTarget + " :They aren't on that channel", fd);
				if (adding)
					channel->addNewOperator(_clients[targetFd]);
				else
					channel->removeOperator(targetFd);
				messageMode += "o";
				messageArgs += " " + targetNick;
			}
			else if (c == 'k')
			{
				if (adding)
				{
					//ERR_NEEDMOREPARAMS
					if (it == _recv.end())
						throw IrcException(461, "MODE", ":Not enough parameters", fd);
					std::string newKey = *it;
					it++;
					//ERR_KEYSET
					if (channel->getHasChannelKey())
						throw IrcException(467, chanTarget, ":Channel key already set", fd);
					channel->setHasChannelKey(true);
					channel->setChannelKey(newKey);
					messageMode += "k";
					messageArgs += " " + newKey;
				}
				else
				{
					//ERR_NEEDMOREPARAMS
					if (it == _recv.end())
						throw IrcException(461, "MODE", ":Not enough parameters", fd);
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
			else if (c == 'l')
			{
				if (adding)
				{
					//ERR_NEEDMOREPARAMS
					if (it == _recv.end())
						throw IrcException(461, "MODE", ":Not enough parameters", fd);
					long limit;
					std::stringstream ss(*it);
					ss >> limit;
					if (!ss.fail() && limit > 0 && limit <= UINT_MAX)
					{
						channel->setUserLimit(static_cast<unsigned int>(limit));
						messageMode += "l";
						messageArgs += " " + *it;
					}
					it++;
				}
				else
				{
					channel->setUserLimit(UINT_MAX);
					messageMode += "l";
				}
			}
			else if (c == 'i')
			{
				channel->setIsInviteOnly(adding);
				messageMode += "i";
			}
			else if (c == 't')
			{
				channel->setHasTopicRestrictions(adding);
				messageMode += "t";
			}
			//ERR_UNKNOWNMODE
			else
				throw IrcException(472, std::string(1, c), ":is unknown mode char to me", fd);
		}
		catch ( IrcException const& e )
		{
			std::string errorMsg = buildMessage(e.getCode(), e.getTarget(), e.getText(), e.getFd());
			send(e.getFd(), errorMsg.c_str(), errorMsg.length(), 0);
		}
	}
	if (messageMode.empty() == false && messageMode != "+" && messageMode != "-")
	{
		std::string fullMsg = ":" + _clients[fd]->getNickname() + " MODE " + chanTarget + " " + messageMode + messageArgs + "\r\n";
		channel->sendMessageToMembers(fullMsg, -1);
	}
}

void	IrcServer::INVITE( int const& fd )
{
	//ERR_NOTREGISTERED
	if (_clients[fd]->getIsRegistered() == false)
		throw IrcException(-1, "INVITE", ":User not registered", fd);
	//ERR_NEEDMOREPARAMS
	if (_recv.size() < 3)
		throw IrcException(461, "INVITE", ":Not enough parameters", fd);
	std::list<std::string>::iterator it = _recv.begin();
	it++;
	std::string targetNick = *it;
	it++;
	std::string chanTarget = *it;
	std::map<int, Client*>::iterator nickIt;
	bool targetFound = false;
	int targetFd = -1;
	for (nickIt = _clients.begin(); nickIt != _clients.end(); ++nickIt)
	{
		if (nickIt->second->getNickname() == targetNick)
		{
			targetFound = true;
			targetFd = nickIt->first;
			break ;
		}
	}
	//ERR_NOSUCHNICK
	if (targetFound == false)
		throw IrcException(401, targetNick, ":No such nick/channel", fd);
	std::map<std::string, Channel*>::iterator chanIt = _channels.find(chanTarget);
	//ERR_NOSUCHCHANNEL
	if (chanIt == _channels.end())
		throw IrcException(403, chanTarget, ":No such channel", fd);
	Channel* channel = chanIt->second;
	//ERR_NOTONCHANNEL
	if (!channel->hasMember(fd))
		throw IrcException(442, chanTarget, ":You're not on that channel", fd);
	//ERR_USERONCHANNEL
	if (channel->hasMember(targetFd))
		throw IrcException(443, targetNick + " " + chanTarget, ":is already on channel", fd);
	//ERR_CHANOPRIVSNEEDED
	if (channel->getIsInviteOnly() && !isAnyOperator(fd, chanTarget))
		throw IrcException(482, chanTarget, ":You're not channel operator", fd);
	_clients[targetFd]->addInvite(chanTarget);
	//RPL_INVITING (341)
	std::string message = ":localhost 341 " + _clients[fd]->getNickname() + " " + targetNick + " " + chanTarget + "\r\n";
	send(fd, message.c_str(), message.length(), 0);
	std::string inviteMsg = ":" + _clients[fd]->getNickname() + " INVITE " + targetNick + " " + chanTarget + "\r\n";
	send(targetFd, inviteMsg.c_str(), inviteMsg.length(), 0);
}

void	IrcServer::OPER( int const& fd )
{
	if (_clients[fd]->getIsRegistered() == false)
		throw IrcException(-1, "OPER", ":User not registered", fd);	
	//ERR_NEEDMOREPARAMS
	if (_recv.size() < 2)
		throw IrcException(461, "OPER", ":Not enough parameters", fd);

	std::list<std::string>::iterator l_it = _recv.begin();
	std::string nickName = *(++l_it);
	int targetFd = getFdfromNickname(nickName);

	if (targetFd == -1)
		throw IrcException(-1, "OPER", ":No such nickname", fd);

	if (_clients[targetFd]->getIsServerOperator())
		throw IrcException(-1, "OPER", ":User is already an operator", fd);

	_clients[targetFd]->setIsOperator(true);

	std::string message = nickName + " is now a server operator.\n";
	send(fd, message.c_str(), message.length(), 0);
}

void	IrcServer::CAP( int const& fd )
{
	std::list<std::string>::iterator it = _recv.begin();
	if (_recv.size() >= 2)
	{
		it++;
		std::string subcommand = *it;

		if (subcommand == "LS")
		{
			std::string response = ":localhost CAP * LS :\r\n";
			send(fd, response.c_str(), response.length(), 0);
		}
		else if (subcommand == "REQ")
		{
			it++;
			std::string requestedCaps = "";
			if (it != _recv.end())
				requestedCaps = *it;

			std::string response = ":localhost CAP * NAK :" + requestedCaps + "\r\n";
			send(fd, response.c_str(), response.length(), 0);
		}
	}
}

void IrcServer::PING( int const& fd )
{
	if (_recv.size() < 2)
		throw IrcException(461, "PING", ":Not enough parameters", fd);

	std::list<std::string>::iterator it = _recv.begin();
	it++;

	std::string response = ":localhost PONG localhost :" + *it + "\r\n";
	send(fd, response.c_str(), response.length(), 0);
}
