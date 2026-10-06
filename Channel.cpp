#include "Channel.hpp"
Channel::Channel( void )
{
	if (DEBUG)
		std::cout << "\033[0;31mDefault Channel constructor called\033[0m" << std::endl;
}

Channel::Channel( std::string name, Client *owner ): _name(name), _channelKey(""), _isInviteOnly(false), _hasTopicRestrictions(false), _hasChannelKey(false)
{
	addNewMember(owner);
	addNewOperator(owner);
	if (DEBUG)
		std::cout << "\033[0;32mChannel parametric constructor called\033[0m" << std::endl;
}

Channel::Channel( Channel const& copy )
{
    *this = copy;
	if (DEBUG)
		std::cout << "\033[0;32mChannel copy constructor called\033[0m" << std::endl;
}

Channel::~Channel( void )
{
	std::map<int, Client*>::const_iterator it;
	
	for (it = _members.begin(); it != _members.end(); ++it)
	{
		it->second->removeChannel(_name);
	}
	if (DEBUG)
		std::cout << "\033[0;31mDefault Channel destructor called\033[0m" << std::endl;
}

//Overload operator=
Channel&	Channel::operator=( Channel const& copy )
{
	if (DEBUG)
		std::cout << "\033[0;32mChannel copy assignment operator called\033[0m" << std::endl;
	if (this != &copy)
	{
        _channelKey = copy._channelKey;
        _isInviteOnly = copy._isInviteOnly;
        _hasTopicRestrictions = copy._hasTopicRestrictions;
        _hasChannelKey = copy._hasChannelKey;
		_members = copy._members;
		_operators = copy._operators;
	}
	return *this;
}

void	Channel::sendMessageToMembers( std::string const& message, int const& sender_fd )
{
	std::map<int, Client*>::const_iterator 	it;	
	for (it = _members.begin(); it != _members.end(); ++it)
    {
		if (it->first != sender_fd)
			send(it->first, message.c_str(), message.length(), 0);
	}
}

void	Channel::addNewMember( Client *newMember )
{
	_members[newMember->getFd()] = newMember;
	if (DEBUG)
		this->displayMembers();
}

//return true if the owner is removed, false otherwise
bool	Channel::removeMember( int const& fd )
{
	_members[fd]->removeChannel(_name);
	_members.erase(fd);
	if (DEBUG)
		this->displayMembers();
	if (_members.empty())
		return true;
	return false;
}

void	Channel::addNewOperator( Client *newOperator )
{
	_operators[newOperator->getFd()] = newOperator;
	if (DEBUG)
		this->displayMembers();
}

//return true if the owner is removed, false otherwise
void	Channel::removeOperator( int const& fd )
{
	_operators.erase(fd);
	if (DEBUG)
		this->displayOperators();
}


void	Channel::displayMembers( void )
{
	std::map<int, Client*>::const_iterator it;
	std::cout << "Members of channel " << _name << std::endl;
    for (it = _members.begin(); it != _members.end(); ++it)
        std::cout << "\tFD: " << it->first << "\tNickname: " << it->second->getNickname() << std::endl;
}

void	Channel::displayOperators( void )
{
	std::map<int, Client*>::const_iterator it;
	std::cout << "Operators of channel " << _name << std::endl;
    for (it = _operators.begin(); it != _operators.end(); ++it)
        std::cout << "\tFD: " << it->first << "\tNickname: " << it->second->getNickname() << std::endl;
}

void	Channel::displayMembers( int const& fd )
{
	std::map<int, Client*>::const_iterator it;
	std::ostringstream oss;
	std::string name;

	std::string message = "Members of channel " + _name + "\n";
	send(fd, message.c_str(), message.length(), 0);

    for (it = _members.begin(); it != _members.end(); ++it)
	{
		name = "";
		if (_operators.find(it->first) != _operators.end())
			name = name + "@";
		name = name + it->second->getNickname();
		oss.str("");
        oss << "\tFD: " << it->first << "\tNickname: " << name << std::endl;
		message = oss.str();
		send(fd, message.c_str(), message.length(), 0);
	}
}

bool Channel::hasMember(int fd) const
{
	return (_members.find(fd) != _members.end());
}

std::map<int, Client*> Channel::getMembers() const
{
	return this->_members;
}
