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