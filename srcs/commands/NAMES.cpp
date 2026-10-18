void	IrcServer::NAMES( int const& fd )
{
	bool notAll;

	basicChecks("NAMES", fd, 1);
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
		if ((*it).empty() == false && isMaskChar((*it)[0]) == false)
			throw IrcException(476, "NAMES " + *it, ":Bad Channel Mask", fd);
		if ((*it).size() == 1)
			throw IrcException(-1, "NAMES " + *it, ":Bad Channel Name", fd);
		std::map<std::string, Channel*>::iterator chan_it = _channels.find(*it);
		if (chan_it == _channels.end())
			throw IrcException(403, "NAMES" + *it, ":No such channel", fd);
		_channels[*it]->displayMembers(fd, notAll);
	}
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