#pragma once
#include <iostream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string>
#include <sstream>
#include <poll.h>
#include <stdbool.h>
#include <cerrno>
#include <cstdlib>
#include <map>
#include <sys/signal.h>
#include <csignal>

extern volatile sig_atomic_t g_running;

class Client;

class Server
{
  public:
	class Channel
	{
		private:
			std::vector<Client *> 		operators;
			std::vector<Client *>		members;
			std::vector<std::string>	invited;
			std::string topic;
			std::string name;
			std::string Password;
			std::string lastTopicSetter;
			bool k; // mdp pour le channel
			bool l; // limit le nb de client
			bool t; // seul un utilisateur peut changer le topic du channel
			bool i; // Définir/supprimer le canal sur invitation uniquement
			bool o; // Donner/retirer le privilège de l’opérateur de canal
			size_t membersLimit;

		public:
			//      GET         //

			bool 						&getK();
			bool 						&getI();
			bool 						&getT();
			bool 						&getL();
			bool 						&getO();
			size_t 						getMembersLimit();
			std::string 				&getTopic();
			std::string 				&getName();
			std::string 				&getPass();
			std::string 				&getLastTopicSetter();//a set
			std::vector<Client *> 		&getMembers();
			std::vector<std::string> 	&getInvitedMembers();

			//      SET         //
			void setK(bool active);
			void setT(bool active);
			void setI(bool active);
			void setL(bool active);
			void setO(bool active);
			void setMembersLimit(size_t limit);
			void setTopic(const std::string &name);
			void setName(const std::string &name);
			void setOperator(Client &client);
			void setOffOperator(Client &client);
			void setPasssword(const std::string &name);
			void setOfMember(std::string name);
			//		OTHERS		//
			
			bool hasBeenInvited(std::string name);
			Channel();
			~Channel();
	};

	public:

		Server(char**av);
		int 	createServer();
		void 	closeAllSockets();
		bool 	checkDoubleName(const char *name);
		void 	sendMessage(std::string buffer, Client client);
		void	addClientInChannel(Server::Channel &channel, Client  &client);
		void 	QUIT();
		//		GET			//

		struct sockaddr_in 		&getSockaddr();
		std::vector<pollfd> 	&getFds();
		std::vector<Client *> 	&getClients();
		std::vector<Channel *> 	&getChannels();
		std::string 			&getPass();
		std::string 			&getCurrentTime();
		int 					&getPort();

		//		SET			//

		void 	setCurrentTime(std::stringstream &ss);
		void 	setPassword(std::string &name);
		void 	setPort(int Port);

	private:
		struct sockaddr_in 		serv_addr;
		int 					srv;
		static bool 			Signal;
		std::vector<Client *> 	clients;
		std::vector<Channel *> 	channels;
		std::vector<pollfd> 	fds;
		int 					port;
		std::string 			pass;
		char 					*buffer;	
		std::string 			currentTime;
};



/////////////////////				UTILS					////////////////////


Server::Channel 			&ChannelMatch(Server server, std::string name);
std::vector<std::string> 	splitCommand(std::string command, char c);
std::string 				extractMessage(char *tmp);
std::string 				extractPass(std::string pass);
std::string 				joinVector(const std::vector<std::string> &vec, char sep);
bool						prohibidedCharacter(std::string tmp);
bool						prohibidedCharacterJoin(std::string tmp, bool checkFirst);
bool 						searchChannelMatch(Server server, std::string name);
bool 						prohibidedCharacterModePassword(std::string word);
bool 						prohibitedCharacterServerPassword(std::string word);
bool 						checkNum(std::string tmp);
bool 						searchMembers(std::string name, Server::Channel channel);
Client 						&clientMatch(std::string name,  Server server);
void 						removeChannelMember(Server::Channel &channel, Client &client);
void 						sendMessageAllClientKick(Server &server, Server::Channel &channel, std::vector<std::string> words, Client client);
std::vector<std::string> 	removeCharacter(std::vector<std::string> vec, char c);
void 						welcomeMessage(Server &server, Server::Channel &channel, Client  &client);
void 						sendMessageAllClient(Server &server, Server::Channel &channel, std::string message);
void						setup_signal_handlers();
int							checkArg(int ac, char **av);
void						sendMessageAllClientJoin(Client &client, Server &server, Server::Channel &channel, std::string message);


/////////////////////				COMMANDES				/////////////////////


void 	MODE(Server &server, Client &client, const char *tmp);
bool	extractAndSetMessageForUser(char *tmp, Client &client, Server &server);
void 	setUserAndNick(Client &client, Server &server, char *buff);
void 	processCommand(Client &client, Server &server, size_t *i);
bool	execCommand(char *buff , Client &tmp, Server &server, size_t *i);
void	JOIN(Client &client, Server &server, const char *tmp);
void    KICK(Server &server, Client &client, const char *tmp);
void	execKick(Server::Channel &channel, Client &target);
void 	INVITE(Client &client, Server &server, const char *tmp);
void 	TOPIC(Server &server, Client &client, const char *tmp);
void 	NICK(Server &server, Client &client, const char *tmp);


/////////////////////				SIGNAUX				/////////////////////


