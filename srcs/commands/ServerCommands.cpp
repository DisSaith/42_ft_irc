void	IrcServer::OPER( int const& fd )
{
	if (_clients[fd]->getIsRegistered() == false)
		throw IrcException(-1, "OPER", ":User not registered", fd);	
	//ERR_NEEDMOREPARAMS
	if (_recv.size() < 2)
		throw IrcException(461, "OPER", ":Not enough parameters", fd);

	std::list<std::string>::iterator l_it = _recv.begin();
	std::string nickName = *(++l_it);
	int targetFd = getFdfromNickname(nickName);

	if (targetFd == -1)
		throw IrcException(-1, "OPER", ":No such nickname", fd);

	if (_clients[targetFd]->getIsServerOperator())
		throw IrcException(-1, "OPER", ":User is already an operator", fd);

	_clients[targetFd]->setIsOperator(true);

	std::string message = nickName + " is now a server operator.\n";
	send(fd, message.c_str(), message.length(), 0);
}

void	IrcServer::CAP( int const& fd )
{
	std::list<std::string>::iterator it = _recv.begin();
	if (_recv.size() >= 2)
	{
		it++;
		std::string subcommand = *it;

		if (subcommand == "LS")
		{
			std::string response = ":localhost CAP * LS :\r\n";
			send(fd, response.c_str(), response.length(), 0);
		}
		else if (subcommand == "REQ")
		{
			it++;
			std::string requestedCaps = "";
			if (it != _recv.end())
				requestedCaps = *it;

			std::string response = ":localhost CAP * NAK :" + requestedCaps + "\r\n";
			send(fd, response.c_str(), response.length(), 0);
		}
	}
}

void IrcServer::PING( int const& fd )
{
	if (_recv.size() < 2)
		throw IrcException(461, "PING", ":Not enough parameters", fd);

	std::list<std::string>::iterator it = _recv.begin();
	it++;

	std::string response = ":localhost PONG localhost :" + *it + "\r\n";
	send(fd, response.c_str(), response.length(), 0);
}