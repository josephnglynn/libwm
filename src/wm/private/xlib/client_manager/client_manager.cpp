//
// Created by joseph on 20/11/2021.
//
#include <X11/Xatom.h>
#include "../../../public/xlib/client_manager/client_manager.hpp"
#include "../../../public/flow_wm_xlib.hpp"

namespace flow::X11
{

	void ClientManager::AddClient(Client* client)
	{
		count++;
		if (!first)
		{
			first = client;
		}

		if (!first->next && first != client)
		{
			first->next = client;
			client->previous = first;
		}

		if (last)
		{
			last->next = client;
			client->previous = last;
		}
		last = client;
	}

	bool ClientManager::Exists(Client* client)
	{
		Client* c = first;

		while (c)
		{
			if (c == client) return true;
			c = c->next;
		}

		return false;
	}

	bool ClientManager::Exists(Window window)
	{
		Client* c = first;

		while (c)
		{
			if (c->window == window) return true;
			c = c->next;
		}

		return false;
	}

	void ClientManager::RemoveClient(Client* client)
	{

		if (client == first)
		{
			if (client->next)
			{
				first = client->next;
			}
			else
			{
				first = nullptr;
			}

		}

		if (client == last)
		{
			if (client->previous)
			{
				last = client->previous;
			}
			else
			{
				last = nullptr;
			}
		}

		if (client->previous) client->previous->next = client->next;
		if (client->next) client->next->previous = client->previous;
		delete client;
		count--;
	}

	void ClientManager::RemoveClient(Window window)
	{
		Client* c = first;

		while (c)
		{
			if (c->window == window) return RemoveClient(c);
			c = c->next;
		}
	}

	unsigned int ClientManager::GetCount() const
	{
		return count;
	}

	Client* ClientManager::GetClient(Window window)
	{
		Client* c = first;

		while (c)
		{
			if (c->window == window) return c;
			c = c->next;
		}

		return nullptr;
	}

	long ClientManager::GetState(Window window)
	{
		auto fwm = FlowWindowManagerX11::Get();
		int format;
		long result = -1;
		unsigned char* p = NULL;
		unsigned long n, extra;
		Atom real;

		if (XGetWindowProperty(fwm->GetDisplay(),
			window,
			fwm->GetWmAtom()[WMState],
			0L,
			2L,
			False,
			fwm->GetWmAtom()[WMState],
			&real,
			&format,
			&n,
			&extra,
			(unsigned char**)&p) != Success)
			return -1;
		if (n != 0)
			result = *p;
		XFree(p);
		return result;
	}



	Client* ClientManager::GetFirst()
	{
		return first;
	}

	Client* ClientManager::GetLast()
	{
		return last;
	}

	void ClientManager::SetFirst(Client* client)
	{
		first = client;
	}

	void ClientManager::SetLast(Client* client)
	{
		last = client;
	}

}

