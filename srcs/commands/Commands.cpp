void	IrcServer::basicChecks(std::string const& functionName, int const& fd, size_t const& size)
{
	if (_clients[fd]->getIsRegistered() == false)
		throw IrcException(-1, functionName, ":User not registered", fd);
	//ERR_NEEDMOREPARAMS
	if (_recv.size() < size)
		throw IrcException(461, functionName, ":Not enough parameters", fd);
}

#include "JOIN.cpp"
#include "PART.cpp"
#include "QUIT.cpp"
#include "KICK.cpp"
#include "NAMES.cpp"
#include "MODE.cpp"
#include "TOPIC.cpp"
#include "INVITE.cpp"
#include "PRIVMSG.cpp"
#include "ServerCommands.cpp"
#include "Registration.cpp"
