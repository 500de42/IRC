#include "../includes/Client.hpp"
#include "../includes/Serveur.hpp"

int	main(int ac, char **av)
{
	struct pollfd server_poll;
	size_t	len;
	ssize_t	bytes;
	char	buff[512];

	if (ac != 3)
	{
		std::cout << "the program must be run like this < ./ircserv <port> <password> >\n";
		return (1);
	}
	if (prohibitedCharacterServerPassword((std::string)av[2]))
	{
		std::cout << "The password contains prohibited characters.\n";
		return (1);
	}
	if (!checkNum((std::string)av[1]) || std::atoi(av[1]) < 1024
		|| std::atoi(av[1]) > 65535)
	{
		std::cout << "The port is not correct\n";
		return (1);
	}
	Server server(av);
	if (server.createServer())
		return (1);
	while (1)
	{
		std::cout << "test reset\n";
		if (poll(&server.getFds()[0], server.getFds().size(), -1) > 0)
		{	if (server.getFds()[0].revents & POLLIN)
			{
				std::cout << "\n\ntest1\n\n";
				Client*client = new Client(server.getPort());
                std::cout << "\n\ntest2\n\n";
				client->setCliSocket(accept(server.getFds()[0].fd, NULL, NULL));
                std::cout << "\n\ntest553\n\n";
				if (client->getSocket() > 0)
				{
					std::cout << "\n\ntest3\n\n";
					server_poll.fd = client->getSocket();
					server_poll.events = POLLIN;
					client->setServsocket(server.getFds()[0].fd);
					server.getClients().push_back(client);
					server.getFds().push_back(server_poll);
				}
                else
                {
                    std::cout << "error addind client in vector\n\n";
                }
			}}
            // std::cout << "\n\ntest4\n\n";
		if (server.getClients().size())
		{
			for (size_t i = 1; i < server.getFds().size(); i++)
			{
				//std::cout << "\n\ntest5\n\n";
				Client &tmp = *server.getClients()[i - 1];
				//std::cout << "\n " << i << " client size : " << server.getClients().size() << "\ntest6\n\n";
				if (server.getFds()[i].revents & POLLIN)
				{
					std::cout << "\n\ntest7 fd\n" ;
					std::cout << " " << tmp.getRegister() << std::endl;
					std::cout << "size of vector client " << server.getClients().size() << std::endl; 
					std::cout << server.getClients()[i - 1]->getSocket() << "\n\n";                
					std::cout << "\n\ntest7.5\n\n";
					memset(buff, 0, 512);
					bytes = recv(server.getFds()[i].fd, buff, sizeof(buff) - 1, 0);
					std::cout << "\n\ntest7.7\n\n";
					if (bytes > 0)
					{		
						tmp.getRealBuffer().append(buff, bytes);
						processCommand(tmp, server, bytes, &i);
					}
					else if (bytes == 0)
					{
						std::cout << "\n\ntest9\n\n";
						std::cout << "Client " << tmp.getSocket() << " déconnecté." << std::endl;
						close(tmp.getSocket());
						server.getClients().erase(server.getClients().begin() + (i - 1));
						server.getFds().erase(server.getFds().begin() + i);
						delete &tmp;
						i--;
						// errno(); client deco
					}
					else
					{
						std::cout << "\n\ntest91\n\n";
						// error
					}
				}
				else if (server.getFds()[i].revents & POLLERR)
				{
					std::cout << "\n\ntest99\n\n";
				}
				else if (server.getFds()[i].revents & POLLHUP)
				{
					std::cout << "\n\ntest999\n\n";
				}
			}
		}
	}
	return (0);
}
