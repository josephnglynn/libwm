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
		bool Exists(Window window);
		unsigned int GetCount() const;
		Client* GetClient(Window window);
		void FocusNull();
		void RemoveClient(Client* client);
		void RemoveClient(Window window);
	private:
		Client* first = nullptr;
		Client* last = nullptr;
		unsigned int count = 0;
	};
}

#endif //FLOW_WM_CLIENT_MANAGER_HPP
