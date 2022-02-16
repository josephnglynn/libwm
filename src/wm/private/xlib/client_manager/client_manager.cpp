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
		unsigned char* p = nullptr;
		unsigned long n, extra;
		Atom real;

		if (
			XGetWindowProperty(
				fwm->GetDisplay(),
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
				(unsigned char**)&p
			) != Success)
		{
			return -1;
		}

		if (n != 0) result = *p;
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

	X11::Client* ClientManager::WindowToClient(Window w)
	{
		Client* c;

		for (c = first; c; c = c->next)
		{
			if (c->window == w) return c;
		}

		return nullptr;
	}

	bool ClientManager::DontTouchWindow(Window w)
	{
		Client* c;

		for (c = first; c; c = c->next)
		{
			if (c->window == w || c->frame == w) return true;
		}

		if (FlowWindowManagerX11::Get()->GetBase() == w) return true;
		return false;
	}

	void ClientManager::Manage(Window window, XWindowAttributes* wa)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();
		auto root = fwm->GetRootWindow();
		Client* t, * client = new Client();
		Window trans = None;
		XWindowChanges wc;

		client->window = window;
		client->position.x = client->old_position.x = wa->x;
		client->position.y = client->old_position.y = wa->y;
		client->position.width = client->old_position.width = wa->width;
		client->position.height = client->old_position.height = wa->height;
		client->old_border_width = wa->border_width;

		client->UpdateTitle();

		if (CheckAtom(client->window, fwm->GetNetAtom()[NetWMWindowType], fwm->GetNetAtom()[NetWMTypeDesk]) ||
			CheckAtom(client->window, fwm->GetNetAtom()[NetWMWindowType], fwm->GetNetAtom()[NetWMTypeDock]))
		{
			client->is_annoying = true;
		}

		if (XGetTransientForHint(display, window, &trans) && (t = WindowToClient(trans)))
		{
			client->monitor = t->monitor;
		}
		else
		{
			client->monitor = fwm->GetScreenManager()->selected_monitor;
			//TODO APPLY RULES HERE WE SHOULD CHECK FOR OUR SHELL
		}

		client->border_width = static_cast<int>(fwm->GetConfig()->border_size_in_pixels) * 0; //TODO MAYBE CHANGE
		wc.border_width = client->border_width;
		client->frame_offsets = fwm->GetShell()->GetOffsets(client->is_annoying);

		Monitor* m = client->monitor;
		if (client->position.width > m->mw) client->position.width = m->mw - 2 * client->border_width;
		shapes::CenterRectangleOnPlane(shapes::Rectangle(m->mx, m->my, m->mw, m->mh), &client->position);

		XUnmapWindow(display, client->window);
		XConfigureWindow(display, window, CWBorderWidth, &wc);
		client->UpdateWindowType();
		client->UpdateSizeHints();
		client->UpdateWmHints();

		XSelectInput(
			display,
			window,
			EnterWindowMask | FocusChangeMask | PropertyChangeMask | StructureNotifyMask
		);

		AddClient(client);

		XChangeProperty(
			display,
			root,
			fwm->GetNetAtom()[NetClientList],
			XA_WINDOW,
			32,
			PropModeAppend,
			(unsigned char*)&(client->window),
			1
		);

		XMoveResizeWindow(
			display,
			client->window,
			client->position.x + static_cast<int>(client->frame_offsets.left),
			client->position.y + static_cast<int>(client->frame_offsets.top),
			client->position.width - client->frame_offsets.right,
			client->position.height - client->frame_offsets.bottom
		);

		client->SetState(NormalState);
		if (client->monitor == fwm->GetScreenManager()->selected_monitor)
		{
			UnFocus(selected, 1);
		}
		selected = client;
		Frame(client);
		XRaiseWindow(display, window);
		Focus(client);


	}

	void ClientManager::Focus(X11::Client* client, bool fromButtonPress)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();
		auto root = fwm->GetRootWindow();

		if (selected && selected != client)
		{
			UnFocus(selected, 1);
		}

		if (client)
		{
			if (client->monitor != fwm->GetScreenManager()->selected_monitor)
				fwm->GetScreenManager()->selected_monitor = client->monitor;
			if (client->is_urgent) client->SetUrgent(0);
			fwm->GetKeyboardManager()->GrabButtons(client, 1);
			client->SetFocus();

#ifdef FLOW_BETTER_FOCUS
			if (fromButtonPress) XRaiseWindow(display, client->frame);
#else
			XRaiseWindow(display, client->frame);
#endif

		}
		else
		{
			Window focus = fwm->GetBase() ? fwm->GetBase() : root;
			XSetInputFocus(display, focus, RevertToPointerRoot, CurrentTime);
			XDeleteProperty(display, focus, fwm->GetNetAtom()[NetActiveWindow]);
		}
		selected = client;
	}

	void ClientManager::UnManage(Client* client, int destroyed)
	{
		auto fwm = X11::FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();

		Monitor* m = client->monitor;
		if (!destroyed)
		{
			XWindowChanges wc;
			wc.border_width = client->old_border_width;
			XGrabServer(display);
			XSetErrorHandler([](Display*, XErrorEvent*) -> int
			{ return 0; });
			UnFrame(client);
			XDestroyWindow(display, client->frame);
			XConfigureWindow(display, client->window, CWBorderWidth, &wc);
			XUngrabButton(display, AnyButton, AnyModifier, client->window);
			client->SetState(WithdrawnState);
			XSync(display, False);
			XSetErrorHandler(FlowX11ErrorHandler);
			XUngrabServer(display);
		}
		RemoveClient(client);
		Focus(nullptr);
		fwm->GetScreenManager()->Arrange(m);
	}

	void ClientManager::UnFocus(X11::Client* client, int set_focus)
	{
		if (!client) return;
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();
		auto root = fwm->GetRootWindow();
		fwm->GetKeyboardManager()->GrabButtons(client, 0);
		if (set_focus)
		{
			XSetInputFocus(display, root, RevertToPointerRoot, CurrentTime);
			XDeleteProperty(display, root, fwm->GetNetAtom()[NetActiveWindow]);
		}

	}

	void ClientManager::Resize(X11::Client* client, int x, int y, int w, int h, int interact)
	{
		if (client->ApplySizeHints(&x, &y, &w, &h, interact)) client->ResizeClient(x, y, w, h);
	}

	void ClientManager::ReparentToBase(Client* client)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto base = fwm->GetBase();

		if (!base) return;

		auto display = fwm->GetDisplay();

		XReparentWindow(display, client->frame, base, client->position.x, client->position.y);
	}

	void ClientManager::Frame(X11::Client* client)
	{
		if (client->is_annoying) return;
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();
		auto root = fwm->GetRootWindow();
		auto shell = fwm->GetShell();

		client->frame = shell->CreateFrame(
			client->position.x,
			client->position.y,
			client->position.width,
			client->position.height,
			display,
			root
		);

		XAddToSaveSet(display, client->window);
		XReparentWindow(display, client->window, client->frame, client->frame_offsets.left, client->frame_offsets.top);
		XMapWindow(display, client->frame);

		fwm->GetKeyboardManager()->GrabButtons(client, 0);
		XMapWindow(display, client->window);
		ReparentToBase(client);
	}

	void ClientManager::UnFrame(X11::Client* client)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();
		auto root = fwm->GetRootWindow();
		XUnmapWindow(display, client->frame);
		XReparentWindow(display, client->window, root, client->position.x, client->position.y);
		XRemoveFromSaveSet(display, client->window);
	}

	bool ClientManager::CheckAtom(Window window, Atom big_atom, Atom small_atom)
	{
		Atom* state;
		unsigned long i, n;
		bool ret = false;

		state = (Atom*)GetAtom(window, big_atom, &n);
		for (i = 0; i < n; i++)
		{
			if (state[i] == small_atom) ret = true;
		}

		XFree(state);
		return ret;
	}

	void* ClientManager::GetAtom(Window window, Atom atom, unsigned long* items)
	{
		int format, status;
		unsigned char* ret = NULL;
		unsigned long extra;
		Atom real;
		Display* dpy = FlowWindowManagerX11::Get()->GetDisplay();

		status = XGetWindowProperty(
			dpy,
			window,
			atom,
			0L,
			64L,
			False,
			AnyPropertyType,
			&real,
			&format,
			items,
			&extra,
			(unsigned char**)&ret
		);

		if (status != Success)
		{
			*items = 0;
			return nullptr;
		}

		return ret;
	}

	Client* ClientManager::GetClientFromFrame(Window window)
	{

		Client* c;

		for (c = first; c; c = c->next)
		{
			if (c->frame == window)
			{
				return c;
			}
		}

		return nullptr;
	}

	void ClientManager::KillClient(Client* client)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();
		if (!client->SendEvent(fwm->GetWmAtom()[WMDelete]))
		{
			XGrabServer(display);
			XSetErrorHandler([](Display*, XErrorEvent*) -> int
			{ return 0; });
			XSetCloseDownMode(display, DestroyAll);
			XKillClient(display, selected->window);
			XSync(display, false);
			XSetErrorHandler(nullptr);
			XUngrabServer(display);
		}
	}

}

