#include "../includes/IRC.hpp"

int Server::createServer()
{
    this->srv = socket(AF_INET, SOCK_STREAM, 0);
    if (this->srv < 0)
        return -1;
    this->Port = htons(6667);
}
