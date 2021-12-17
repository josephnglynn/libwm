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
				if (IsUniqueGeom(unique, j, &info[i]))
					memcpy(&unique[j++], &info[i], sizeof(XineramaScreenInfo));
			XFree(info);
			nn = j;
			if (n <= nn)
			{ /* new monitors available */
				for (i = 0; i < (nn - n); i++)
				{
					for (m = mons; m && m->next; m = m->next);
					if (m)
						m->next = CreateMonitor();
					else
						mons = CreateMonitor();
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
			if (unique[n].x_org == info->x_org && unique[n].y_org == info->y_org
				&& unique[n].width == info->width && unique[n].height == info->height)
				return 0;
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
			if ((a = Intersect(rectangle.x, rectangle.y, rectangle.width, rectangle.height, m)) > area)
			{
				area = a;
				r = m;
			}
		return r;
	}

	X11::Client* ScreenManager::WindowToClient(Window w)
	{
		Client* c;
		Monitor* m;

		for (m = mons; m; m = m->next)
			for (c = m->clients->GetFirst(); c; c = c->next)
				if (c->window == w)
					return c;
		return nullptr;
	}

	void ScreenManager::Manage(Window window, XWindowAttributes* wa)
	{
		auto fwm = FlowWindowManagerX11::Get();
		Client* t;
		Client* client = new Client();
		Window trans = None;
		XWindowChanges wc;

		client->window = window;
		client->position.x = client->old_position.x = wa->x;
		client->position.y =client->old_position.y = wa->y;
		client->position.width = client->old_position.width = wa->width;
		client->position.height = client->old_position.height =wa->height;
		client->old_border_width = wa->border_width;

		if (XGetTransientForHint(fwm->GetDisplay(), window, &trans) && (t = WindowToClient(trans)))
		{
			client->monitor = t->monitor;
		}
		else
		{
			client->monitor = selected_monitor;
			//TODO APPLY RULES HERE WE SHOULD CHECK FOR OUR SHELL
		}

		/*int width_b = Width(client);
		int height_b = Height(client);

		if (client->position.x + width_b > client->monitor->mx + client->monitor->mw)
		{
			client->position.x = client->monitor->mx + client->monitor->mw - width_b;
		}

		if (client->position.y + height_b > client->monitor->my + client->monitor->mh)
		{
			client->position.y = client->monitor->my + client->monitor->mh - height_b;
		}

		client->position.x = Max(client->position.x, client->monitor->mx);

		client->position.y = Max(client->position.y,
			((client->monitor->by == client->monitor->my)
				&& (client->position.x + (static_cast<int>(client->position.width) / 2) >= client->monitor->wx)
				&& (client->position.x + (static_cast<int>(client->position.width) / 2)
					< client->monitor->wx + client->monitor->ww)) ? 0
																  : client->monitor->my
		);*/


		client->border_width = fwm->GetConfig()->border_size_in_pixels;
		wc.border_width = client->border_width;


		Monitor* m = client->monitor;
		if (client->position.width > m->mw) client->position.width = m->mw - 2 * client->border_width;
		shapes::CenterRectangleOnPlane(shapes::Rectangle(m->mx, m->my, m->mw, m->mh), &client->position);

		XConfigureWindow(fwm->GetDisplay(), window, CWBorderWidth, &wc);
		XSetWindowBorder(fwm->GetDisplay(), window, fwm->GetColorScheme()[SchemeNorm][ColBorder].pixel);
		client->Configure();
		client->UpdateWindowType();
		client->UpdateSizeHints();
		client->UpdateWmHints();
		XSelectInput(fwm->GetDisplay(),
			window,
			EnterWindowMask | FocusChangeMask | PropertyChangeMask | StructureNotifyMask
		);
		fwm->GetKeyboardManager()->GrabButtons(client, 0);
		XRaiseWindow(fwm->GetDisplay(), window);
		client->monitor->clients->AddClient(client);
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
			client->position.x,
			client->position.y,
			client->position.width,
			client->position.height
		);//TODO COME BACK HERE IF X POS IS MESSED UP
		client->SetState(NormalState);
		if (client->monitor == selected_monitor)
		{
			UnFocus(GetSelectedMonitor()->clients->selected, 1);
		}
		client->monitor->clients->selected = client;
		XMapWindow(fwm->GetDisplay(), client->window);
		Focus(nullptr);
	}

	void ScreenManager::Focus(X11::Client* client)
	{
		auto fwm = FlowWindowManagerX11::Get();
		if (selected_monitor->clients->selected && selected_monitor->clients->selected != client)
			UnFocus(selected_monitor->clients->selected, 1);
		if (client)
		{
			if (client->monitor != selected_monitor)
				selected_monitor = client->monitor;
			if (client->is_urgent) client->SetUrgent(0);
			fwm->GetKeyboardManager()->GrabButtons(client, 1);
			XSetWindowBorder(fwm->GetDisplay(), client->window, fwm->GetColorScheme()[SchemeSel][ColBorder].pixel);
			client->SetFocus();
		}
		else
		{
			XSetInputFocus(fwm->GetDisplay(), fwm->GetRootWindow(), RevertToPointerRoot, CurrentTime);
			XDeleteProperty(fwm->GetDisplay(), fwm->GetRootWindow(), fwm->GetNetAtom()[NetActiveWindow]);
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
		Monitor* m = client->monitor;
		if (!destroyed)
		{
			XWindowChanges wc;
			wc.border_width = client->old_border_width;
			XGrabServer(fwm->GetDisplay());
			XSetErrorHandler([](Display*, XErrorEvent*) -> int
			{ return 0; });
			XConfigureWindow(fwm->GetDisplay(), client->window, CWBorderWidth, &wc);
			XUngrabButton(fwm->GetDisplay(), AnyButton, AnyModifier, client->window);
			client->SetState(WithdrawnState);
			XSync(fwm->GetDisplay(), False);
			XSetErrorHandler(FlowX11ErrorHandler);
			XUngrabServer(fwm->GetDisplay());
		}
		m->clients->RemoveClient(client);
		fwm->GetScreenManager()->Focus(nullptr);
		Arrange(m);
	}

	void ScreenManager::UnFocus(X11::Client* client, int set_focus)
	{
		if (!client) return;
		auto fwm = FlowWindowManagerX11::Get();
		fwm->GetKeyboardManager()->GrabButtons(client, 0);
		XSetWindowBorder(fwm->GetDisplay(), client->window, fwm->GetColorScheme()[SchemeNorm][ColBorder].pixel);
		if (set_focus)
		{
			XSetInputFocus(fwm->GetDisplay(), fwm->GetRootWindow(), RevertToPointerRoot, CurrentTime);
			XDeleteProperty(fwm->GetDisplay(), fwm->GetRootWindow(), fwm->GetNetAtom()[NetActiveWindow]);
		}

	}

	void ScreenManager::Resize(X11::Client* client, int x, int y, int w, int h, int interact)
	{
		if (client->ApplySizeHints(&x, &y, &w, &h, interact)) client->ResizeClient(x, y, w, h);
	}

	void ScreenManager::ReStack(Monitor* m)
	{
		auto fwm = FlowWindowManagerX11::Get();
		Client* c = m->clients->selected;
		XEvent ev;

		if (!c)
			return;
		XRaiseWindow(fwm->GetDisplay(), c->window);
		XSync(fwm->GetDisplay(), False);
		while (XCheckMaskEvent(fwm->GetDisplay(), EnterWindowMask, &ev));
	}

	void ScreenManager::Arrange(Monitor* m)
	{
		if (m) ReStack(m);
	}

}