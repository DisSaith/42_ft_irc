void	IrcServer::PART( int const& fd )
{
	basicChecks("PART", fd, 2);

	std::list<std::string>::iterator l_it = _recv.begin();
	std::string channelName = *(++l_it);
	std::string target = "PART " + channelName;
	std::map<std::string, Channel*>::iterator chan_it = _channels.find(channelName);

	if (channelName.empty() == false && isMaskChar((channelName)[0]) == false)
		throw IrcException(476, target, ":Bad Channel Mask", fd);
	if (channelName.size() == 1)
		throw IrcException(-1, target, ":Bad Channel Name", fd);
	//ERR_NOSUCHCHANNEL
	if (chan_it == _channels.end())
		throw IrcException(403, target, ":No such channel", fd);
	//ERR_NOTONCHANNEL
	if (_clients[fd]->getChannels().find(channelName) == _clients[fd]->getChannels().end())
		throw IrcException(461, target, ":You're not on that channel", fd);

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