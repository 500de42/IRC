#pragma once
#include "Serveur.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <poll.h>
#include <sstream>
#include <stdbool.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class Client
{
  private:
	struct sockaddr_in cli_addr;
	std::string ipAddress;
	int cliSocket;
	int servSocket;
	std::string nickname;
	std::string username;
	std::string realname;
	std::string realBuffer;
	char Buffer[512];
	bool goodPass;
	bool isRegistred;
	std::vector<Server::Channel *> channels;
	std::map<std::string, bool> channelOp;

  public:
	//      GET     //

	bool getOp(std::string name);
	bool getGivenPassword();
	int getServsocket();
	int getSocket();
	char *getBuffer();
	std::string &getRealBuffer();
	struct sockaddr_in &getSockaddr();
	bool getRegister();
	std::string &getNickname();
	std::string &getUsername();
	std::string &getRealname();
	std::vector<Server::Channel *> &getChannels();
	std::map<std::string, bool> &getOpMap();

	//      SET     //

	void setServsocket(int nb);
	void setCliSocket(int nb);
	void setBuffer(char *buffer);
	void setNickname(const char *name);
	void setUsername(const char *name);
	void setRealname(const char *name);
	void onRegisted();
	void onPass();
	void setOp(std::string name, bool mode);
	void setOffOpMap(std::string name);
	Client(int port);
	
	//		OTHER		//

	void quitAllChannels(Server server);
	Client &operator=(const Client &cpy);
};
