void	IrcServer::TOPIC( int const& fd )
{
    basicChecks("TOPIC", fd, 2);

	std::string	message;
	std::list<std::string>::iterator recv_it = _recv.begin();
	std::string channelName = *(++recv_it);
	++recv_it;
	std::string topic;
	for (; recv_it != _recv.end(); recv_it++)
	{
		if (!topic.empty())
			topic += " ";
		topic += *recv_it;
	}

	if (channelName.empty() == false && isMaskChar(channelName[0]) == false)
		throw IrcException(476, "TOPIC " + channelName, ":Bad Channel Mask", fd);
	if (channelName.size() == 1)
		throw IrcException(-1, "TOPIC " + channelName, ":Bad Channel Name", fd);
	//ERR_NOSUCHCHANNEL
	if (_channels.find(channelName) == _channels.end())
		throw IrcException(403, channelName, ":No such channel", fd);
	if (!_channels[channelName]->hasMember(fd))
		throw IrcException(442, channelName, ":You're not on that channel", fd);
	if (_recv.size() == 2)
	{
		if (_channels[channelName]->getTopic().empty())
			message = ":localhost 331 " + _clients[fd]->getNickname() + " " + channelName + " :No topic is set\r\n";
		else
			message = ":localhost 332 " + _clients[fd]->getNickname() + " " + channelName + " :" + _channels[channelName]->getTopic() + "\r\n";
		send(fd, message.c_str(), message.length(), 0);
	}
	else
	{
		if (_channels[channelName]->getHasTopicRestrictions() && !isAnyOperator(fd, channelName))
			throw IrcException(482, channelName, ":You're not channel operator", fd);
		_channels[channelName]->setTopic(topic);
		message = ":" + _clients[fd]->getAllNames() + " TOPIC " + channelName + " :" + _channels[channelName]->getTopic() +"\r\n";
		_channels[channelName]->sendMessageToMembers(message, -1);
	}
}
