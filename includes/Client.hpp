#pragma once
#include "Serveur.hpp"
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

class Client
{
    private:
        struct sockaddr_in cli_addr;
        std::string ipAddress;
        int cliSocket;
        int servSocket;
        std::map<std::string, bool> channelOp;
        std::string nickname;
        std::string username;
        std::string realname;
        char *Buffer;
        bool isRegistred;
        std::vector<Server::Channel*> channels;

    public:
        //      GET     //

        std::string getIpAddr();
        int getFd();
        bool getOp(std::string name);
        int getServsocket();
        int getSocket();
        char * getBuffer();
        struct sockaddr_in &getSockaddr();
        bool getRegister();
        std::string &getNickname();
        std::string &getUsername();
        std::string &getRealname();
        std::vector<Server::Channel*> &getChannels();

        //      SET     //

        void setServsocket(int nb);
        void setCliSocket(int nb);
        void setBuffer(char *buffer);
        void setNickname(const char *name);
        void setUsername(const char *name);
        void setRealname(const char *name);
        void onRegisted();
        void setOp(std::string name, bool mode);
        Client();
        //              //

        // Client(bool o);
        Client &operator=(const Client &cpy);
        bool extractMessageForUser(char *tmp);
        void sendMessage(const char *buffer);
};
