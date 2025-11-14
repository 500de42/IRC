#include "../includes/Client.hpp"
#include "../includes/Serveur.hpp"

volatile sig_atomic_t g_running = 1;

int	main(int ac, char **av)
{
	ssize_t	bytes;
	char	buff[512];
	time_t startT = time(NULL);
	std::stringstream ss;
	ss << startT;

	if (checkArg(ac, av))
		return 1;
	setup_signal_handlers();
	Server server(av);
	if (server.createServer())
		return (1);
	server.setCurrentTime(ss);
	while (g_running)
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
					struct pollfd server_poll;
					std::cout << "\n\ntest3\n\n";
					memset(&server_poll, 0, sizeof(pollfd));
					server_poll.fd = client->getSocket();
					server_poll.events = POLLIN;
					client->setServsocket(server.getFds()[0].fd);
					server.getClients().push_back(client);
					server.getFds().push_back(server_poll);
				}
				else
					std::cout << "error addind client in vector\n\n";//CLOSE LE SOCKET ACQUIS VIA ACCEPT
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
						processCommand(tmp, server, &i);
					}
					else if (bytes == 0) //client deco
					{
						std::cout << "\n\ntest9\n\n";
						std::cout << "Client " << tmp.getSocket() << " déconnecté." << std::endl;
						
						if (!tmp.getChannels().empty())
						{
							for(std::vector<Server::Channel *>::iterator it = tmp.getChannels().begin(); it != tmp.getChannels().end(); it++)
							{
								try
								{
									Server::Channel &channel = ChannelMatch(server, (*it)->getName());
									// if(tmp.getOp((*it)->getName()))
									// 	channel.setOffOperator(tmp);
									channel.setOfMember(tmp.getNickname());
									//nommer un autre client OP si celui ci est le seul dans le vecteur
								}
								catch(std::exception &e)
								{}
							}
						}
						server.getClients().erase(server.getClients().begin() + (i - 1));
						server.getFds().erase(server.getFds().begin() + i);
						close(tmp.getSocket());
						delete &tmp;
						i--;
					}
					else // error
					{
						std::cout << "\n\ntest91\n\n";
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
	server.QUIT();
	return (0);
}
