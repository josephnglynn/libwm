//
// Created by joseph on 20/11/2021.
//
#include "client_manager.hpp"
#include <iostream>
namespace flow::X11
{

	ClientManager* ClientManager::instance;

	void ClientManager::AddClient(Client* client)
	{
		count++;
		if (!first)
		{
			first = client;
			return;
		}

		if (last) last->next = client;
		last = client;
	}

	bool ClientManager::Exists(Client* client)
	{
		Client* c = first;

		while (c)
		{
			if (c == client)
			{ return true; }
			c = c->next;
		}

		return false;
	}

	bool ClientManager::Exists(Window& window)
	{
		Client* c = first;

		while (c)
		{
			if (c->window == window)
			{ return true; }
			c = c->next;
		}

		return false;
	}

	void ClientManager::RemoveClient(Client* client)
	{

		if (client == first)
		{
			first = client->next;
		}

		if (client == last)
		{
			last = client->previous;
		}

		if (client->previous) client->previous->next = client->next;
		if (client->next) client->next->previous = client->previous;
		delete client;
		count--;
	}

	ClientManager* ClientManager::Get()
	{
		if (!ClientManager::instance)
		{
			ClientManager::instance = new ClientManager();
		}

		return ClientManager::instance;
	}

	void ClientManager::RemoveClient(Window window)
	{
		Client* c = first;

		while (c)
		{
			if (c->window == window) break;
			c = c->next;
		}

		if (!c) return;

		RemoveClient(c);
	}

	unsigned int ClientManager::GetCount()
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

}

