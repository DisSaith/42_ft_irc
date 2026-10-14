#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include "Utils.hpp"

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

int main(int argc, char* argv[])
{
	if (argc != 5)
	{
		std::cerr << "Usage: " << argv[0] << " <IP> <PORT> <PASSWORD> <BOT NAME>" << std::endl;
		return 1;
	}
	std::string	ip = argv[1];
	int			port = Utils::stoi(argv[2]);
	std::string	password = argv[3];
	std::string	bot_name = argv[4];
	int			sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		std::cerr << "Socket error" << std::endl;
		return 1;
	}
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	inet_pton(AF_INET, ip.c_str(), &server.sin_addr);
	if (inet_pton(AF_INET, ip.c_str(), &server.sin_addr) <= 0)
	{
		std::cerr << "Invalid IP address format: " << ip << std::endl;
		close(sock);
		return 1;
	}
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == -1)
	{
		std::cerr << "Error: Unable to connect to " << ip << ":" << port << std::endl;
		close(sock);
		return 1;
	}
	std::cout << "Connected to server. Authenticating..." << std::endl;
	if (!password.empty())
	{
		sendData(sock, "PASS " + password + "\r\n");
	}
	sendData(sock, "NICK " + bot_name + "\r\n");
	sendData(sock, "USER " + bot_name + " 0 * :" + bot_name + " C++\r\n");
	char buffer[4096];
	while (true)
	{
		int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
		if (bytes_received <= 0)
		{
			std::cerr << "Connection lost with the server." << std::endl;
			break;
		}
		buffer[bytes_received] = '\0';
		std::string data(buffer);
		std::cout << data;
		if (data.find(" 464 ") != std::string::npos)
		{
			std::cerr << "\n[FATAL ERROR] Incorrect password (464)." << std::endl;
			break ;
		}
		if (data.find(" 433 ") != std::string::npos)
		{
			std::cerr << "\n[FATAL ERROR] Bot name '" << bot_name << "' is already in use on this server." << std::endl;
			std::cerr << "Please restart the program with a different name." << std::endl;
			break ;
		}
		if (data.find(" 001 ") != std::string::npos)
			std::cout << "Authentication successful! Name validated." << std::endl;
	}
	close(sock);
	return 0;
}
