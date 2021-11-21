//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_CLIENT_MANAGER_HPP
#define FLOW_WM_CLIENT_MANAGER_HPP
#include "../client/client.hpp"

namespace flow::X11
{
	class ClientManager
	{
	public:
		void AddClient(Client* client);
		bool Exists(Client* client);
		bool Exists(Window& window);
		unsigned int GetCount();
		Client* GetClient(Window window);
		void RemoveClient(Client* client);
		void RemoveClient(Window window);
		static ClientManager* Get();
	private:
		Client* first;
		Client* last;
		unsigned int count = 0;
		static ClientManager* instance;
	};
}

#endif //FLOW_WM_CLIENT_MANAGER_HPP
