//
// Created by joseph on 20/11/2021.
//
#include <X11/Xatom.h>
#include "public/xlib/client_manager/client_manager.hpp"
#include "public/flow_wm_xlib.hpp"

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

	void ClientManager::FocusNull()
	{
		auto fwm = FlowWindowManagerX11::Get();
		XSetInputFocus(fwm->GetDisplay(), fwm->GetRootWindow(), RevertToPointerRoot, CurrentTime);
		XDeleteProperty(fwm->GetDisplay(), fwm->GetRootWindow(), fwm->GetNetAtom()[NetActiveWindow]);
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

	void ClientManager::Manage(Window window, XWindowAttributes* wa)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto client = new Client();
		XWindowChanges wc;
		client->window = window;
		client->position.x = wa->x;
		client->position.y = wa->y;
		client->position.width = wa->width;
		client->position.height = wa->height;
		client->old_position = client->position;

		XConfigureWindow(fwm->GetDisplay(), window, CWBorderWidth, &wc);
		XSetWindowBorder(fwm->GetDisplay(), window, fwm->GetColorScheme()[SchemeNorm][ColBorder].pixel);
		client->Configure();
		client->UpdateSizeHints();
		client->UpdateWmHints();
		XSelectInput(fwm->GetDisplay(),
			window,
			EnterWindowMask | FocusChangeMask | PropertyChangeMask | StructureNotifyMask
		);
		client->GrabButtons(0);
		XRaiseWindow(fwm->GetDisplay(), window);
		AddClient(client);
		XChangeProperty(fwm->GetDisplay(),
			fwm->GetRootWindow(),
			fwm->GetNetAtom()[NetClientList],
			XA_WINDOW,
			32,
			PropModeAppend,
			(unsigned char*)&(client->window),
			1
		);
		XMoveResizeWindow(fwm->GetDisplay(),
			client->window,
			client->position.x + 2 * 100,
			client->position.y,
			client->position.width,
			client->position.height
		);//TODO COME BACK HERE IF X POS IS MESSED UP
		client->SetState(NormalState);
		XMapWindow(fwm->GetDisplay(), client->window);
		FocusNull();
	}

	void ClientManager::UnManage(Client* client, int destroyed)
	{
		auto fwm = X11::FlowWindowManagerX11::Get();
		if (!destroyed)
		{
			XWindowChanges wc;
			wc.border_width = 0;
			XGrabServer(fwm->GetDisplay());
			//XSetErrorHandler(xerrordummy);
			XConfigureWindow(fwm->GetDisplay(), client->window, CWBorderWidth, &wc);
			XUngrabButton(fwm->GetDisplay(), AnyButton, AnyModifier, client->window);
			client->SetState(WithdrawnState);
			XSync(fwm->GetDisplay(), False);
			//XSetErrorHandler(xerror);
			XUngrabServer(fwm->GetDisplay());
		}
		RemoveClient(client);
	}

}

