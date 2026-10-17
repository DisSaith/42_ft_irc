void	IrcServer::MODE( int const& fd )
{
	basicChecks("MODE", fd, 2);
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