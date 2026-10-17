void	IrcServer::INVITE( int const& fd )
{
	basicChecks("INVITE", fd, 3);
	std::list<std::string>::iterator it = _recv.begin();
	it++;
	std::string targetNick = *it;
	it++;
	std::string chanTarget = *it;
	std::map<int, Client*>::iterator nickIt;
	bool targetFound = false;
	int targetFd = -1;
	for (nickIt = _clients.begin(); nickIt != _clients.end(); ++nickIt)
	{
		if (nickIt->second->getNickname() == targetNick)
		{
			targetFound = true;
			targetFd = nickIt->first;
			break ;
		}
	}
	//ERR_NOSUCHNICK
	if (targetFound == false)
		throw IrcException(401, targetNick, ":No such nick/channel", fd);
	std::map<std::string, Channel*>::iterator chanIt = _channels.find(chanTarget);
	//ERR_NOSUCHCHANNEL
	if (chanIt == _channels.end())
		throw IrcException(403, chanTarget, ":No such channel", fd);
	Channel* channel = chanIt->second;
	//ERR_NOTONCHANNEL
	if (!channel->hasMember(fd))
		throw IrcException(442, chanTarget, ":You're not on that channel", fd);
	//ERR_USERONCHANNEL
	if (channel->hasMember(targetFd))
		throw IrcException(443, targetNick + " " + chanTarget, ":is already on channel", fd);
	//ERR_CHANOPRIVSNEEDED
	if (channel->getIsInviteOnly() && !isAnyOperator(fd, chanTarget))
		throw IrcException(482, chanTarget, ":You're not channel operator", fd);
	_clients[targetFd]->addInvite(chanTarget);
	//RPL_INVITING (341)
	std::string message = ":localhost 341 " + _clients[fd]->getNickname() + " " + targetNick + " " + chanTarget + "\r\n";
	send(fd, message.c_str(), message.length(), 0);
	std::string inviteMsg = ":" + _clients[fd]->getNickname() + " INVITE " + targetNick + " " + chanTarget + "\r\n";
	send(targetFd, inviteMsg.c_str(), inviteMsg.length(), 0);
}