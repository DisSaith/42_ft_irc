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