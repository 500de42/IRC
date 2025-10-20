#pragma once
#include <Client.hpp>
#include <Channel.hpp>
#include <map>
// class Channel;

class Server
{
    public:
        class Channel;
        int createServer();
        struct sockaddr_in &getSockaddr();
        std::vector<pollfd> &getFds();
        std::vector<Client> &getClients();
        std::vector<Channel> &getChannels();
        void closeAllSockets();
        void registerClient();
        bool checkDoubleName(const char *name);
        void sendMessage(std::string buffer, Client client);

    private:
        struct sockaddr_in serv_addr;
        int srv = -1;
        static bool Signal;
        std::vector<Client> clients;
        std::vector<Channel> channels;
        std::vector<pollfd> fds;
        char *buffer;
};

class Server::Channel
{
    private:
        Client First;
        std::vector<Client> members;
        std::string topic;
        bool k = false;
        bool l = false;
        bool t = false;
        bool i = false;
        std::map<std::string, bool> Password;
        int membersLimit = 0;
    public:
    //      GET         //

        bool &getK();
        bool &getI();
        bool &getT();
        bool &getL();
        std::string &getTopic();
        std::map<std::string, bool> &getPass();
        std::vector<Client> &getMembers();
    
    //      SET         //

        void setK(bool &active);
        void setT(bool &active);
        void setI(bool &active);
        void setL(bool &active);
        void setTopic(const std::string &name);
        void setFirst(Client &client);

};

std::string extractMessage(char *tmp);
bool extractAndSetMessageForUser(char *tmp, Client &client, Server &server);
void setUserAndNick(Client &client, Server &server);
std::string joinVector(const std::vector<std::string> &vec, char sep);
bool prohibidedCharacter(std::string tmp);
bool prohibidedCharacterJoin(std::string tmp);

