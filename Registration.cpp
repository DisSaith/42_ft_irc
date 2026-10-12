void	IrcServer::PASS(int const& fd)
{
	if (_recv.empty() == false && _recv.front() == "PASS")
	{
		//ERR_NEEDMOREPARAMS
		if (_recv.size() < 2)
			throw IrcException(461, "PASS", ":Not enough parameters", fd);
		std::list<std::string>::iterator it = _recv.begin();
		it++;
		if (*it != _password)
		{
			std::string errorMsg = ":localhost 464 * :Password incorrect\r\n";
			send(fd, errorMsg.c_str(), errorMsg.length(), 0);
			return ;
		}
		else if ( DEBUG )
			std::cout << "PASS \"" << *it << "\" " << GREEN << "CORRECT\r\n" << RESET;
		_clients[fd]->setPass();
	}
}

void	IrcServer::NICK(int const& fd)
{
	if (_recv.empty() == false && _recv.front() == "NICK" && _clients[fd]->getHasSetPass() == true)
	{
		//ERR_NEEDMOREPARAMS
		if (_recv.size() < 2)
			throw IrcException(461, "NICK", ":Not enough parameters", fd);
		std::list<std::string>::iterator it = _recv.begin();
		it++;
		for (std::map<int, Client*>::iterator mapIt = _clients.begin(); mapIt != _clients.end(); ++mapIt)
		{
			if ( *it == mapIt->second->getNickname() )
			{
				std::string errorMsg = ":localhost 433 * " + *it + " :Nickname is already in use\r\n";
				send(fd, errorMsg.c_str(), errorMsg.length(), 0);
				return ;
			}
		}
		_clients[fd]->setNickName(*it);
	}
}

void	IrcServer::USER(int const& fd)
{
	if (_clients[fd]->getHasSetPass() && _recv.front() == "USER")
	{
		//ERR_NEEDMOREPARAMS
		if (_recv.size() < 5)
			throw IrcException(461, "USER", ":Not enough parameters", fd);
		std::list<std::string>::iterator it = _recv.begin();
		it++;
		_clients[fd]->setUserName(*it);
		std::advance(it, 3);
		_clients[fd]->setRealName(*it);
		if (_clients[fd]->setIsRegistered())
		{
			std::string welcome = ":localhost 001 " + _clients[fd]->getNickname() + " :Welcome to the ft_irc network!\r\n";
			send(fd, welcome.c_str(), welcome.length(), 0);
			std::cout << "Le client " << fd << " est maintenant officiellement enregistré !" << std::endl;
		} 
	}
}