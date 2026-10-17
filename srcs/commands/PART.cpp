void	IrcServer::PART( int const& fd )
{
	basicChecks("PART", fd, 2);

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