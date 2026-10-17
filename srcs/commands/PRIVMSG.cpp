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