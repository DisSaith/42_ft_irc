void	IrcServer::checkChannelMode( int const& fd, std::string const& channelName, std::list<std::string> &channelPasswords, std::list<std::string>::iterator const& pass_it )
{
    Channel* channel = _channels[channelName];

	if (channel->hasMember(fd))	
        throw IrcException(-1, channelName, ":User already on channel", fd);
	//ERR_INVITEONLYCHAN
	if (channel->getIsInviteOnly() && _clients[fd]->getIsInvited(channelName) == false)
		throw IrcException(473, channelName, ":Cannot join channel (+i)", fd);
	//ERR_BADCHANNELKEY: missing key
	if (pass_it == channelPasswords.end() && channel->getHasChannelKey())
		throw IrcException(475, channelName, ":Cannot join channel (+k)", fd);
	//ERR_BADCHANNELKEY
	if (pass_it != channelPasswords.end() && channel->getHasChannelKey() && (*pass_it).compare(channel->getChannelKey()) != 0)
		throw IrcException(475, channelName, ":Cannot join channel (+k)", fd);
	if (channel->getMembers().size() >= channel->getUserLimit())
		throw IrcException(471, channelName, ":Cannot join channel (+l)", fd);
}

void	IrcServer::JOIN( int const& fd )
{
	basicChecks("JOIN", fd, 2);
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
				checkChannelMode(fd, *it, channelPasswords, pass_it);
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
