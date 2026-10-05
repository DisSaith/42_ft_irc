#include "Channel.hpp"
Channel::Channel( void )
{
	if (DEBUG)
		std::cout << "\033[0;31mDefault Channel constructor called\033[0m" << std::endl;
}

Channel::Channel( std::string name, Client *owner ): _name(name), _channelKey(""), _isInviteOnly(false), _hasTopicRestrictions(false), _hasChannelKey(false),  _owner(owner)
{
	_members[owner->getFd()] = owner;
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
	std::string message = _members[fd]->getNickname() + " leaved the channel.\n";
	sendMessageToMembers( message, -1 );
	_members[fd]->removeChannel(_name);
	_members.erase(fd);
	if (DEBUG)
		this->displayMembers();
	if (fd == _owner->getFd())
		return true;
	return false;
}

void	Channel::displayMembers( void )
{
	std::map<int, Client*>::const_iterator it;
	std::cout << "Members of channel " << _name << std::endl;
    for (it = _members.begin(); it != _members.end(); ++it)
        std::cout << "\tFD: " << it->first << "\tNickname: " << it->second->getNickname() << std::endl;
}

void	Channel::displayMembers( int const& fd )
{
	std::map<int, Client*>::const_iterator it;
	std::ostringstream oss;

	std::string message = "Members of channel " + _name + "\n";
	send(fd, message.c_str(), message.length(), 0);

    for (it = _members.begin(); it != _members.end(); ++it)
	{
        oss << "\tFD: " << it->first << "\tNickname: " << it->second->getNickname() << std::endl;
		message = oss.str();
		send(fd, message.c_str(), message.length(), 0);
	}
}