#include "Bot.cpp"

bool	sendData(int sock, const std::string& msg)
{
	int total_sent = 0;
	int length = msg.length();
	while (total_sent < length)
	{
		int bytes_sent = send(sock, msg.c_str() + total_sent, length - total_sent, MSG_NOSIGNAL);
		if (bytes_sent == -1)
		{
			std::cerr << "Send error." << std::endl;
			return false;
		}
		total_sent += bytes_sent;
	}
	return true;
}

std::list<std::string> readDictionary()
{
	std::string current_line;
	std::list<std::string> output;
	std::ifstream inFile("./bonus/dictionary");

	if (!inFile.is_open())
	{
		std::cerr << YELLOW << "Warning: " << RESET << "Unable to open dictionary" << std::endl;
		return output;
	}
	while (getline (inFile, current_line)) 
	{
		if (!current_line.empty())
		{
			if (current_line[current_line.length() - 1] == '\r')
				current_line.erase(current_line.length() - 1);
  			output.push_back(current_line);
		}
	}
	inFile.close();

	if (output.empty())
		std::cerr << YELLOW << "Warning: " << RESET << " Empty dictionary" << std::endl;

	return output;
}

bool checkMessage(std::list<std::string> message, std::list<std::string> const& dictionary, char *buffer)
{
	bool found = false;
    std::string word;

	for (std::list<std::string>::iterator message_it = message.begin(); message_it != message.end(); message_it++)
	{
        word = *message_it;
        if (word[0] == ':')
            word.erase(0, 1);
		if (std::find(dictionary.begin(), dictionary.end(), word) != dictionary.end())
		{
			found = true;
			std::cout << RED << "Bad Language: " << RESET << buffer;
		}
	}
	return found;
}

bool	connectBot(Bot const& bot)
{
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(bot.getPort());
	inet_pton(AF_INET, bot.getIp().c_str(), &server.sin_addr);
	if (inet_pton(AF_INET, bot.getIp().c_str(), &server.sin_addr) <= 0)
	{
		std::cerr << "Invalid IP address format: " << bot.getIp() << std::endl;
		close(bot.getSocket());
		return false;
	}
	if (connect(bot.getSocket(), (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		std::cerr << "Error: Unable to connect to " << bot.getIp() << ":" << bot.getPort() << std::endl;
		close(bot.getSocket());
		return false;
	}
	std::cout << "Connected to server. Authenticating..." << std::endl;
	if (!bot.getPassword().empty())
		sendData(bot.getSocket(), "PASS " + bot.getPassword() + "\r\n");
	sendData(bot.getSocket(), "NICK " + bot.getBotname() + "\r\n");
	sendData(bot.getSocket(), "USER " + bot.getBotname() + " 0 * :" + bot.getBotname() + " C++\r\n");
	return true;
}

bool	checkErrorMessage(std::list<std::string> const& data, Bot const& bot)
{
	if (std::find(data.begin(), data.end(), "464") != data.end())
	{
		std::cerr << "\n[FATAL ERROR] Incorrect password (464)." << std::endl;
		return true ;
	}
	if (std::find(data.begin(), data.end(), "432") != data.end())
	{
		std::cerr << "\n[FATAL ERROR] Bot Name is too long." << std::endl;
		return true ;
	}
	if (std::find(data.begin(), data.end(), "433") != data.end())
	{
		std::cerr << "\n[FATAL ERROR] Bot name '" << bot.getBotname() << "' is already in use on this server." << std::endl;
		std::cerr << "Please restart the program with a different name." << std::endl;
		return true ;
	}
	if (std::find(data.begin(), data.end(), "001") != data.end())
		std::cout << "Authentication successful! Name validated." << std::endl;
    if (std::find(data.begin(), data.end(), ":Cannot join channel") != data.end())
		sendData(bot.getSocket(), "PRIVMSG " + bot.getLastInviteNickname() + " Cannot join channel" + "\r\n");

	return false;
}

void	checkCommandMessage(std::list<std::string> const& data, Bot &bot, std::list<std::string> const& dictionary, char *buffer)
{
	std::list<std::string>::const_iterator data_it = std::find(data.begin(), data.end(), "PRIVMSG");
	if (data_it != data.end())
	{
		data_it++;
		if (((*(data_it))[0] == '&' || (*(data_it))[0] == '#') && checkMessage(data, dictionary, buffer))
		{
			std::string BadUserName = data.front();		
			BadUserName.erase(0, 1);
			sendData(bot.getSocket(), "PRIVMSG " + BadUserName + " :WARNING BAD LANGUAGE" + "\r\n");
			bot.incrementUserWarnings(BadUserName);
			std::cout << BadUserName << " is at warning number "; 
               std::cout << bot.getUsersWarnings()[BadUserName] << "/" << bot.getMaxNbWarnings() << "." << std::endl;
			if (bot.getUsersWarnings()[BadUserName] > bot.getMaxNbWarnings())
				sendData(bot.getSocket(), "KICK " + *data_it + " " + BadUserName + " :Bad Language" + "\r\n");
		}
	}
    if (std::find(data.begin(), data.end(), "INVITE") != data.end())
	{
		std::vector<std::string> vdata(data.begin(), data.end());
		bot.setLastInviteNickname(vdata[0]);
		sendData(bot.getSocket(), "JOIN " + vdata[3] + "\r\n");
	}
}

bool	getBuffer(Bot const& bot, char *buffer, size_t const& size)
{
	int bytes_received = recv(bot.getSocket(), buffer, size - 1, 0);
	if (bytes_received <= 0)
	{
		std::cerr << "Connection lost with the server." << std::endl;
		return false;
	}
	buffer[bytes_received] = '\0';
	std::cout << buffer;
	return true;
}

void    addToLog(std::ofstream &ofs, char *buffer)
{
	if (ofs.is_open())
	{
		char timeBuf[42];
		time_t now = time(NULL);
		if (ctime_r(&now, timeBuf) != NULL)
		{
			std::string timestamp(timeBuf);
			//ctime add a \n at the end that I remove
			if (timestamp.empty() == false && timestamp[timestamp.length() - 1] == '\n')
				timestamp.erase(timestamp.length() - 1);
			ofs << "[" << timestamp << "]: "<< buffer;
		}
		ofs.flush();
	}
}

int main(int argc, char* argv[])
{ 
	if (argc != 6)
	{
		std::cerr << "Usage: " << argv[0] << " <IP> <PORT> <PASSWORD> <BOT NAME> <MAX NB OF WARNINGS>" << std::endl;
		return 1;
	}

	Bot bot(argv);
	
	if (bot.getSocket() == -1)
	{
		std::cerr << "Socket error" << std::endl;
		return 1;
	}
	if (!connectBot(bot))
        return 1;
	
	char buffer[4096];
	std::list<std::string> dictionary = readDictionary();
	std::ofstream ofs("log", std::ios::trunc);
	if (!ofs.is_open())
		std::cerr << YELLOW << "Warning: " << RESET << "Unable to open log file" << std::endl;
	
	sendData(bot.getSocket(), "OPER " + bot.getBotname() + "\r\n");
	std::list<std::string> data;
	while (true)
	{
		if (!getBuffer(bot, buffer, 4096))
			break ;
        data = Utils::split(buffer, ' ');
		if (checkErrorMessage(data, bot))
			break ;
        checkCommandMessage(data, bot, dictionary, buffer);
        addToLog(ofs, buffer);
	}
	close(bot.getSocket());
	return 0;
}
