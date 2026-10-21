void    IrcServer::QUIT( int const& fd )
{
	if (_recv.front() == "QUIT")
	{
		std::string quitMsg = "Client Quit";
		if (_recv.size() > 1)
		{
			std::list<std::string>::iterator it = _recv.begin();
			it++;
			quitMsg = "";
			while (it != _recv.end())
			{
				quitMsg += *it;
				it++;
				if (it != _recv.end())
					quitMsg += " ";
			}
			if (!quitMsg.empty() && quitMsg[0] == ':')
				quitMsg.erase(0, 1);
		}
		std::string fullMsg = ":" + _clients[fd]->getNickname() + " QUIT :" + quitMsg + "\r\n";
		std::set<int> clientsToNotify;
		for (std::map<std::string, Channel*>::iterator chanIt = _channels.begin(); chanIt != _channels.end(); )
		{
			if (chanIt->second->hasMember(fd))
			{
				std::map<int, Client*> members = chanIt->second->getMembers();
				
				for (std::map<int, Client*>::iterator memIt = members.begin(); memIt != members.end(); ++memIt)
				{
					if (memIt->first != fd)
						clientsToNotify.insert(memIt->first);
				}
				if (chanIt->second->removeMember(fd))
				{
					delete chanIt->second;
					_channels.erase(chanIt++);
					continue ;
				}
			}
			++chanIt;
		}
		for (std::set<int>::iterator setIt = clientsToNotify.begin(); setIt != clientsToNotify.end(); ++setIt)
		{
			send(*setIt, fullMsg.c_str(), fullMsg.length(), 0);
		}
		_clients[fd]->setIsDisconnect(true);
	}
}
