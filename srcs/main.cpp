#include "../includes/Client.hpp"
#include "../includes/Serveur.hpp"
#include <fcntl.h>

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
		if (poll(&server.getFds()[0], server.getFds().size(), -1) > 0)
		{	if (server.getFds()[0].revents & POLLIN)
			{
				Client*client = new Client(server.getPort());
				client->setCliSocket(accept(server.getFds()[0].fd, NULL, NULL));
				if (client->getSocket() > 0)
				{
					fcntl(client->getSocket(), F_SETFL, O_NONBLOCK);
					struct pollfd server_poll;
					memset(&server_poll, 0, sizeof(pollfd));
					server_poll.fd = client->getSocket();
					server_poll.events = POLLIN;
					client->setServsocket(server.getFds()[0].fd);
					server.getClients().push_back(client);
					server.getFds().push_back(server_poll);
				}
				else
					delete client;
			}}
		if (server.getClients().size())
		{
			for (size_t i = 1; i < server.getFds().size(); i++)
			{
				Client &tmp = *server.getClients()[i - 1];
				if (server.getFds()[i].revents & POLLIN)
				{
					memset(buff, 0, 512);
					bytes = recv(server.getFds()[i].fd, buff, sizeof(buff) - 1, 0);
					if (bytes > 0)
					{
						tmp.getRealBuffer().append(buff, bytes);
						processCommand(tmp, server, &i);
					}
					else if (bytes == 0)
					{
						std::cout << "Client " << tmp.getSocket() << " disconnected." << std::endl;
						
						if (!tmp.getChannels().empty())
						{
							for(std::vector<Server::Channel *>::iterator it = tmp.getChannels().begin(); it != tmp.getChannels().end(); it++)
							{
								try
								{
									Server::Channel &channel = ChannelMatch(server, (*it)->getName());
									channel.setOfMember(tmp.getNickname());
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
				}
			}
		}
	}
	server.QUIT();
	return (0);
}
