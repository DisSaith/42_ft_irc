#include "../includes/Channel.hpp"

/* ======================== Constructors & Destructors ======================== */

Channel::Channel( void )
{
	if (DEBUG)
		std::cout << YELLOW << "[Channel " << _name << "]" << GREEN
				<< " default constructor called" << RESET << std::endl;
}

Channel::Channel( std::string const& name, Client *owner ): _name(name), _channelKey(""), _isInviteOnly(false), _hasTopicRestrictions(false), _hasChannelKey(false), _userLimit(UINT_MAX)
{
	addNewMember(owner);
	addNewOperator(owner);
	if (DEBUG)
	{
		std::cout << YELLOW << "[Channel " << _name << "]" << GREEN
					<< " parametric constructor called" << RESET << std::endl
					<< "Channel user limit: " << _userLimit << std::endl << std::endl;
	}
}

Channel::Channel( Channel const& copy )
{
    *this = copy;
	if (DEBUG)
		std::cout << YELLOW << "[Channel " << _name << "]" << GREEN
					<< " copy constructor called" << RESET << std::endl;
}

Channel::~Channel( void )
{
	for (std::map<int, Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		it->second->removeChannel(_name);
	}
	if (DEBUG)
		std::cout << YELLOW << "[Channel " << _name << "]" << BLUE
			<< " destructor called" << RESET << std::endl;
}

//Overload operator=
Channel&	Channel::operator=( Channel const& copy )
{
	if (DEBUG)
		std::cout << GREEN << "Channel copy assignment operator called" << RESET << std::endl;
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
	
	for (std::map<int, Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
    {
		if (it->first != sender_fd)
			send(it->first, message.c_str(), message.length(), 0);
	}
}

void	Channel::addNewMember( Client *newMember )
{
	_members[newMember->getFd()] = newMember;
	if (DEBUG)
		displayChannel();
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
		this->displayOperators();
}

//return true if the owner is removed, false otherwise
void	Channel::removeOperator( int const& fd )
{
	_operators.erase(fd);
	if (DEBUG)
		this->displayOperators();
}

/* ======================== Display member functions ======================== */

void	Channel::displayChannel( void )
{
	displayMode();
	displayOperators();
	displayMembers();
}

void	Channel::displayMembers( void )
{
	std::cout << "Members of channel " << _name << std::endl;
    for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); ++it)
        std::cout << "\tFD: " << it->first << "\tNickname: " << it->second->getNickname() << std::endl;
}

void	Channel::displayOperators( void )
{
	std::cout << "Operators of channel " << _name << std::endl;
    for (std::map<int, Client*>::const_iterator it = _operators.begin(); it != _operators.end(); ++it)
        std::cout << "\tFD: " << it->first << "\tNickname: " << it->second->getNickname() << std::endl;
}

void	Channel::displayMembers( int const& fd , bool const& displayEndOfNames )
{
	std::string message;
	std::string name;

	message = _name + " :";
    for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); ++it)
	{
		name = "";
		if (_operators.find(it->first) != _operators.end())
			name = name + "@";
		name = name + it->second->getNickname();
		message = message + name;
		if (++it != _members.end())
        	message = message + " ";
		--it;
	}
	message = message + "\n";
	send(fd, message.c_str(), message.length(), 0);
	if (displayEndOfNames)
	{
		message = _name + " :End of /NAMES list\n";
		send(fd, message.c_str(), message.length(), 0);
	}
}

void	Channel::displayTopic( void ) const
{
	std::string message = _name + " :";
	if (_topic.empty())
		message = message + "No topic is set\n";
	else
		message = message + _topic;
	std::cout << message << std::endl;
}

void	Channel::displayTopic( int const& fd ) const
{
	std::string message = _name + " :";
	if (_topic.empty())
		message = message + "No topic is set\n";
	else
		message = message + _topic;
	send(fd, message.c_str(), message.length(), 0);
}

void	Channel::displayMode ( void ) const
{
	std::string	message = "---CHANNEL " + _name + "---\n";

	message += "TOPIC      :" + _topic + "\n";
	message += "USER LIMIT :" + ::itos(_userLimit) + "\n";
	message += "KEY        :" + _channelKey + "\n";
	message += "Mode (+k)  : " + ::itos(static_cast<int>(_hasChannelKey)) + "\n";
	message += "Mode (+t)  : " + ::itos(static_cast<int>(_hasTopicRestrictions)) + "\n";
	message += "Mode (+i)  : " + ::itos(static_cast<int>(_isInviteOnly)) + "\n";

	std::cout << message << std::endl;
}

/* ======================== Default Constructor & Destructor ======================== */

bool Channel::hasMember( int fd ) const
{
	return (_members.find(fd) != _members.end());
}

std::map<int, Client*> Channel::getMembers() const
{
	return this->_members;
}

bool	Channel::getIsChannelOperator( int const& fd )
{
	if (_operators.find(fd) != _operators.end())
		return true;
	return false;
}

bool	Channel::getIsInviteOnly() const
{
	return this->_isInviteOnly;
}

bool	Channel::getHasChannelKey() const
{
	return this->_hasChannelKey;
}

std::string	Channel::getChannelKey() const
{
	return this->_channelKey;
}

unsigned int	Channel::getUserLimit() const
{
	return this->_userLimit;
}

std::string		Channel::getTopic() const
{
	return this->_topic;
}

void	Channel::setHasChannelKey( bool status )
{
	this->_hasChannelKey = status;
	if (DEBUG)
		displayMode();
}

void	Channel::setChannelKey( std::string const& key )
{
	this->_channelKey = key;
	if (DEBUG)
		displayMode();
}

void	Channel::setUserLimit( unsigned int const& limit )
{
	this->_userLimit = limit;
	if (DEBUG)
		displayMode();
}

bool	Channel::getHasTopicRestrictions() const
{
	return this->_hasTopicRestrictions;
}

void Channel::setIsInviteOnly( bool status )
{
	this->_isInviteOnly = status;
	if (DEBUG)
		displayMode();
}

void Channel::setHasTopicRestrictions( bool status )
{
	this->_hasTopicRestrictions = status;
	if (DEBUG)
		displayMode();
}

void Channel::setTopic( std::string const& newTopic )
{
	_topic = newTopic;
	if (DEBUG)
		displayMode();
}

void Channel::freeChannels()
{
	for (std::map<int, Client*>::iterator it = _members.begin(); it != _members.end(); ++it)
		it->second->removeChannel(_name);
}
