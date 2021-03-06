#pragma once

#include "../../UDPServer/UDPServer/server.h"

namespace sb
{

class Game_client
{
public:
	Game_client() { initialize(); }
	Game_client(const Game_client&) = delete;
	Game_client& operator=(const Game_client&) = delete;
	Game_client(Game_client&&) = delete;
	Game_client& operator=(Game_client&&) = delete;
	~Game_client() = default;

	void initialize()
	{
		server.start(Udp_address{ "127.0.0.1", 27016 }, Udp_config{512}, &login_server_adress);

		std::thread network_listen([&] {server.listen_loop(); });
		std::thread network_send([&] {server.resend_loop(); });
		std::thread logic([&] {run(); });
		std::thread console([&] {run_console(); });

		logic.join();
		network_listen.join();
		network_send.join();
		console.join();
	}

	using Message = std::vector<char>;

private:

	void run_console()
	{
		while (running)
		{
			std::string command;
			std::getline(std::cin, command);

			if (command == "exit")
			{
				running = false;
				server.terminate();
			}

			if (command == "login")
			{
				server.send(login_server_adress, make_package("Please log me in"));
			}

			if(command.find("say ") == 0)
			{
				server.send(login_server_adress, make_package(command.substr(4, command.size() - 4)));
			}
		}
	}

	void run()
	{
		while (running)
		{
			if (server.has_package())
			{
				auto package = server.deque();
				std::cout << "Package from " << package.source.hostname << ":"
					<< std::to_string(package.source.port) << ", content:\n" << package.message.data() << "\n";
			}
		}
	}

	Udp_address login_server_adress
	{
		"127.0.0.1",
		27015
	};
	std::atomic_bool running{ true };
	Game_udp_server server;

};

}