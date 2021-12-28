//
// Created by joseph on 20/11/2021.
//

#include "../../../public/flow_wm_xlib.hpp"
#include "../../../public/general/input_functions.hpp"
#include <X11/Xatom.h>
#include <xlib/screens/screens.hpp>
#include "../../../public/general/inline_functions.hpp"

namespace flow
{

	using namespace X11;

	int ScreenManager::UpdateGeom()
	{
		auto fwm = FlowWindowManagerX11::Get();

		int dirty = 0;

		if (XineramaIsActive(fwm->GetDisplay()))
		{
			int i, j, n, nn;
			Client* c;
			Monitor* m;
			XineramaScreenInfo* info = XineramaQueryScreens(fwm->GetDisplay(), &nn);
			XineramaScreenInfo* unique;

			for (n = 0, m = mons; m; m = m->next, n++);
			unique = static_cast<XineramaScreenInfo*>(calloc(nn, sizeof(XineramaScreenInfo)));
			for (i = 0, j = 0; i < nn; i++)
			{
				if (IsUniqueGeom(unique, j, &info[i]))
				{
					memcpy(&unique[j++], &info[i], sizeof(XineramaScreenInfo));
				}
			}
			XFree(info);

			nn = j;
			if (n <= nn)
			{ /* new monitors available */
				for (i = 0; i < (nn - n); i++)
				{
					for (m = mons; m && m->next; m = m->next);
					if (m)
					{
						m->next = CreateMonitor();
					}

					else
					{
						mons = CreateMonitor();
					}

				}
				for (i = 0, m = mons; i < nn && m; m = m->next, i++)
					if (i >= n
						|| unique[i].x_org != m->mx || unique[i].y_org != m->my
						|| unique[i].width != m->mw || unique[i].height != m->mh)
					{
						dirty = 1;
						m->num = i;
						m->mx = m->wx = unique[i].x_org;
						m->my = m->wy = unique[i].y_org;
						m->mw = m->ww = unique[i].width;
						m->mh = m->wh = unique[i].height;
						m->screen = unique[i].screen_number;
					}
			}
			else
			{ /* less monitors available nn < n */
				for (i = nn; i < n; i++)
				{
					for (m = mons; m && m->next; m = m->next);
					while ((c = m->clients->GetFirst()))
					{
						dirty = 1;
						m->clients->SetFirst(c->next);
						c->monitor = mons;
					}
					if (m == selected_monitor) selected_monitor = mons;
					CleanUpMonitor(m);
				}
			}
			free(unique);
		}
		else
		{ /* default monitor setup */
			int sw = fwm->GetScreenWidth();
			int sh = fwm->GetScreenHeight();
			if (!mons) mons = CreateMonitor();
			if (mons->mw != sw || mons->mh != sh)
			{
				dirty = 1;
				mons->mw = mons->ww = sw;
				mons->mh = mons->wh = sh;
			}
		}
		if (dirty)
		{
			selected_monitor = mons;
			selected_monitor = WindowToMonitor(fwm->GetRootWindow());
		}
		return dirty;

	}

	int ScreenManager::IsUniqueGeom(XineramaScreenInfo* unique, size_t n, XineramaScreenInfo* info)
	{
		while (n--)
		{
			if (unique[n].x_org == info->x_org && unique[n].y_org == info->y_org && unique[n].width == info->width
				&& unique[n].height == info->height)
			{
				return 0;
			}
		}
		return 1;
	}

	Monitor* ScreenManager::CreateMonitor()
	{
		Monitor* m;
		m = static_cast<Monitor*>(calloc(1, sizeof(Monitor)));
		m->clients = new ClientManager();
		return m;
	}

	void ScreenManager::CleanUpMonitor(Monitor* monitor)
	{
		if (monitor == mons)
		{
			mons = mons->next;
		}
		else
		{
			Monitor* m;
			for (m = mons; m && m->next != monitor; m = m->next);
			m->next = monitor->next;
		}
		free(monitor->clients);
		free(monitor);
	}

	Monitor* ScreenManager::WindowToMonitor(Window w)
	{
		auto fwm = FlowWindowManagerX11::Get();
		int x, y;
		Client* c;

		if (w == fwm->GetRootWindow() && input_functions::GetRootPointer(&x, &y))
			return RectToMonitor(shapes::Rectangle(x, y, 1, 1));
		if ((c = WindowToClient(w)))
			return c->monitor;
		return selected_monitor;
	}
	Monitor* ScreenManager::RectToMonitor(shapes::Rectangle rectangle)
	{
		Monitor* m, * r = selected_monitor;
		int a, area = 0;

		for (m = mons; m; m = m->next)
		{
			if ((a = Intersect(rectangle.x, rectangle.y, rectangle.width, rectangle.height, m)) > area)
			{
				area = a;
				r = m;
			}
		}

		return r;
	}

	X11::Client* ScreenManager::WindowToClient(Window w)
	{
		Client* c;
		Monitor* m;

		for (m = mons; m; m = m->next)
		{
			for (c = m->clients->GetFirst(); c; c = c->next)
			{
				if (c->window == w) return c;
			}
		}

		return nullptr;
	}

	bool ScreenManager::DontTouchWindow(Window w)
	{
		Client* c;
		Monitor* m;

		for (m = mons; m; m = m->next)
		{
			for (c = m->clients->GetFirst(); c; c = c->next)
			{
				if (c->window == w || c->frame == w)return true;
			}
		}

		return false;
	}

	void ScreenManager::Manage(Window window, XWindowAttributes* wa)
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
			client->monitor = selected_monitor;
			//TODO APPLY RULES HERE WE SHOULD CHECK FOR OUR SHELL
		}

		client->border_width = fwm->GetConfig()->border_size_in_pixels * 0; //TODO MAYBE CHANGE
		wc.border_width = client->border_width;
		client->frame_offsets = fwm->GetShell()->GetOffsets(client->is_annoying);

		Monitor* m = client->monitor;
		if (client->position.width > m->mw) client->position.width = m->mw - 2 * client->border_width;
		shapes::CenterRectangleOnPlane(shapes::Rectangle(m->mx, m->my, m->mw, m->mh), &client->position);

		XUnmapWindow(display, client->window);
		XConfigureWindow(display, window, CWBorderWidth, &wc);
		XSetWindowBorder(display, window, fwm->GetColorScheme()[SchemeNorm][ColBorder].pixel);
		client->Configure();
		client->UpdateWindowType();
		client->UpdateSizeHints();
		client->UpdateWmHints();

		XSelectInput(
			display,
			window,
			EnterWindowMask | FocusChangeMask | PropertyChangeMask | StructureNotifyMask
		);

		XRaiseWindow(display, window);
		client->monitor->clients->AddClient(client);

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
			client->position.x + client->frame_offsets.left,
			client->position.y + client->frame_offsets.top,
			client->position.width - client->frame_offsets.right,
			client->position.height - client->frame_offsets.bottom
		);

		client->SetState(NormalState);
		if (client->monitor == selected_monitor)
		{
			UnFocus(GetSelectedMonitor()->clients->selected, 1);
		}
		client->monitor->clients->selected = client;
		Frame(client);
		Focus(nullptr);
	}

	void ScreenManager::Focus(X11::Client* client)
	{
		auto fwm = FlowWindowManagerX11::Get();
		if (selected_monitor->clients->selected && selected_monitor->clients->selected != client)
			UnFocus(selected_monitor->clients->selected, 1);
		if (client)
		{
			if (client->monitor != selected_monitor) selected_monitor = client->monitor;
			if (client->is_urgent) client->SetUrgent(0);
			fwm->GetKeyboardManager()->GrabButtons(client, 1);
			client->SetFocus();
		}
		selected_monitor->clients->selected = client;
	}

	Monitor* ScreenManager::GetSelectedMonitor()
	{
		return selected_monitor;
	}

	Monitor* ScreenManager::GetMons()
	{
		return mons;
	}

	void ScreenManager::SetSelectedMonitor(Monitor* monitor)
	{
		selected_monitor = monitor;
	}

	void ScreenManager::UnManage(Client* client, int destroyed)
	{
		auto fwm = X11::FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();
		Monitor* m = client->monitor;
		if (!destroyed)
		{
			XWindowChanges wc;
			wc.border_width = client->old_border_width;
			XGrabServer(fwm->GetDisplay());
			XSetErrorHandler([](Display*, XErrorEvent*) -> int
			{ return 0; });
			XUnmapWindow(display, client->frame);
			XReparentWindow(display, client->window, fwm->GetRootWindow(), 0, 0);
			XRemoveFromSaveSet(display, client->window);
			XDestroyWindow(display, client->frame);
			XConfigureWindow(display, client->window, CWBorderWidth, &wc);
			XUngrabButton(display, AnyButton, AnyModifier, client->window);
			client->SetState(WithdrawnState);
			XSync(display, False);
			XSetErrorHandler(FlowX11ErrorHandler);
			XUngrabServer(display);
		}
		m->clients->RemoveClient(client);
		fwm->GetScreenManager()->Focus(nullptr);
		Arrange(m);
	}

	void ScreenManager::UnFocus(X11::Client* client, int set_focus)
	{
		if (!client) return;
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();
		auto root = fwm->GetRootWindow();
		fwm->GetKeyboardManager()->GrabButtons(client, 0);
		XSetWindowBorder(display, client->window, fwm->GetColorScheme()[SchemeNorm][ColBorder].pixel);
		if (set_focus)
		{
			XSetInputFocus(display, root, RevertToPointerRoot, CurrentTime);
			XDeleteProperty(display, root, fwm->GetNetAtom()[NetActiveWindow]);
		}

	}

	void ScreenManager::Resize(X11::Client* client, int x, int y, int w, int h, int interact)
	{
		if (client->ApplySizeHints(&x, &y, &w, &h, interact)) client->ResizeClient(x, y, w, h);
	}

	void ScreenManager::ReStack(Monitor* m)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();
		Client* c = m->clients->selected;
		XEvent ev;

		if (!c)
			return;
		XRaiseWindow(display, c->window);
		XSync(display, False);
		while (XCheckMaskEvent(display, EnterWindowMask, &ev));
	}

	void ScreenManager::Arrange(Monitor* m)
	{
		if (m)
		{
			ReStack(m);
		}
	}

	Monitor* ScreenManager::GetCursorMonitor()
	{
		int x, y;
		input_functions::GetRootPointer(&x, &y);
		return GetMonitor(x, y);
	}

	Monitor* ScreenManager::GetMonitor(int x, int y)
	{
		for (Monitor* m = mons; m; m = m->next)
		{
			if ((x >= m->mx && x <= m->mx + m->mw) && (y >= m->my && y <= m->my + m->mh))
			{
				return m;
			}
		}

		return nullptr;
	}

	void ScreenManager::Frame(X11::Client* client)
	{
		if (client->is_annoying) return;
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();
		auto root = fwm->GetRootWindow();
		auto shell = fwm->GetShell();

		client->frame = shell->CreateWindow(
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
	}

	bool ScreenManager::CheckAtom(Window window, Atom big_atom, Atom small_atom)
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

	void* ScreenManager::GetAtom(Window window, Atom atom, unsigned long* items)
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

	Client* ScreenManager::GetClientFromFrame(Window window)
	{

		Monitor* m;
		Client* c;

		for (m = mons; m; m = m->next)
		{
			for (c = m->clients->GetFirst(); c; c = c->next)
			{
				if (c->frame == window) {
					return c;
				}
			}
		}

		return nullptr;
	}

}