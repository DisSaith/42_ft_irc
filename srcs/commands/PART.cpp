void	IrcServer::PART( int const& fd )
{
	basicChecks("PART", fd, 2);

	std::list<std::string>::iterator l_it = _recv.begin();
	std::list<std::string> channelNames = split(*(++l_it), ',');
	std::string target;
	std::map<std::string, Channel*>::iterator chan_it;
	
	for (std::list<std::string>::iterator it = channelNames.begin(); it != channelNames.end(); it++)
	{
		std::string channelName = *it;
		target = "PART " + channelName;
		chan_it = _channels.find(channelName);
		try
		{
			if ((channelName).empty() == false && isMaskChar((channelName)[0]) == false)
				throw IrcException(476, target, ":Bad Channel Mask", fd);
			if (channelName.size() == 1)
				throw IrcException(-1, target, ":Bad Channel Name", fd);
			//ERR_NOSUCHCHANNEL
			if (chan_it == _channels.end())
				throw IrcException(403, target, ":No such channel", fd);
			//ERR_NOTONCHANNEL
			if (_clients[fd]->getChannels().find(channelName) == _clients[fd]->getChannels().end())
				throw IrcException(461, target, ":You're not on that channel", fd);

			std::string message = ":" + _clients[fd]->getAllNames() + " PART " + channelName + "\r\n";
			_channels[channelName]->sendMessageToMembers( message, -1 );
			if (_channels[channelName]->removeMember(fd))
			{
				delete _channels[channelName];
				_channels.erase(channelName);
			}
		}
		catch ( IrcException const& e )
		{
			std::string message = buildMessage(e.getCode(), e.getTarget(), e.getText(), e.getFd());
			send(e.getFd(), message.c_str(), message.length(), 0);
		}
	}
}