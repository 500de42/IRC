#pragma once
#include "Client.hpp"

class Server
{
  public:
	class Channel
	{
		private:
			std::vector<Client *> operators;
			std::vector<Client *> members;
			std::string topic;
			std::string name;
			std::string Password;
			bool k; // mdp pour le channel
			bool l; // limit le nb de client
			bool t; // seul un utilisateur peut changer le topic du channel
			bool i; // Définir/supprimer le canal sur invitation uniquement
			bool o; // Donner/retirer le privilège de l’opérateur de canal
			size_t membersLimit;

		public:
			//      GET         //

			bool &getK();
			bool &getI();
			bool &getT();
			bool &getL();
			bool &getPasswordMode();
			size_t getMembersLimit();
			std::string &getTopic();
			std::string &getName();
			std::string &getPass();
			std::vector<Client *> &getMembers();

			//      SET         //

			void setK(bool active);
			void setT(bool active);
			void setI(bool active);
			void setL(bool active);
			void setMembersLimit(size_t limit);
			void setTopic(const std::string &name);
			void setName(const std::string &name);
			void setOperator(Client &client);
			void setOffOperator(Client &client);
			void setPasssword(const std::string &name);
			Channel();
	};

	public:

		Server();
		int createServer();
		struct sockaddr_in &getSockaddr();
		std::vector<pollfd> &getFds();
		std::vector<Client *> &getClients();
		std::vector<Channel *> &getChannels();
		void closeAllSockets();
		void registerClient();
		bool checkDoubleName(const char *name);
		void sendMessage(std::string buffer, Client client);


	private:
		struct sockaddr_in serv_addr;
		int srv;
		static bool Signal;
		std::vector<Client *> clients;
		std::vector<Channel *> channels;
		std::vector<pollfd> fds;
		char *buffer;	
};

std::string extractMessage(char *tmp);
bool	extractAndSetMessageForUser(char *tmp, Client &client, Server &server);
void	setUserAndNick(Client &client, Server &server);
std::string joinVector(const std::vector<std::string> &vec, char sep);
bool	prohibidedCharacter(std::string tmp);
bool	prohibidedCharacterJoin(std::string tmp, bool checkFirst);
void	JOIN(Client &client, Server &server, const char *tmp);
Server::Channel &ChannelMatch(Server server, std::string name);
std::vector<std::string> splitCommand(std::string command, char c);
bool searchChannelMatch(Server server, std::string name);

