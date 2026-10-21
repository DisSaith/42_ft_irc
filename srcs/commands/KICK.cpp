void	IrcServer::checkUserAndChannelKick( int const& operator_fd, int const& kicked_fd, std::string const& channelName, std::string const& kickedName)
{
	//ERR_NOSUCHCHANNEL
	if (_channels.find(channelName) == _channels.end())
		throw IrcException(403, "KICK " + channelName, ":No such channel", operator_fd);
	//ERR_NOSUCHNICK
	if (_clients.find(kicked_fd) == _clients.end())
		throw IrcException(401, "KICK " + kickedName, ":No such nick/channel", operator_fd);
	//ERR_CHANOPRIVSNEEDED
	if (!isAnyOperator(operator_fd, channelName))
		throw IrcException(482, "KICK " + channelName, ":You're not channel operator", operator_fd);
	//ERR_NOTONCHANNEL
	if (_clients[kicked_fd]->getChannels().find(channelName) == _clients[kicked_fd]->getChannels().end())
		throw IrcException(441, "KICK", ":User not on that channel", operator_fd);
}

void	IrcServer::KICK( int const& operator_fd )
{
	basicChecks("KICK", operator_fd, 3);

	std::list<std::string>::iterator it = _recv.begin();
	std::string channelName = *(++it);
	std::string nickName = *(++it);
	std::string comment;
	++it;
	for (; it != _recv.end(); it++)
	{
		comment += *it;
		if (it != _recv.end())
			comment += " ";
	}
	int		kicked_fd = getFdfromNickname(nickName);
	checkUserAndChannelKick( operator_fd, kicked_fd, channelName, nickName);

	std::string message = ":" + _clients[operator_fd]->getAllNames() + " KICK " + channelName + " " + _clients[kicked_fd]->getNickname();
	if (!comment.empty() && comment != " ")
		message += " :" + comment;
	message +=  "\r\n";

	_channels[channelName]->sendMessageToMembers( message, -1 );
	if (_channels[channelName]->removeMember(kicked_fd))
	{
		delete _channels[channelName];
		_channels.erase(channelName);
	}
}