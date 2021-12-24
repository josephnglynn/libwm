//
// Created by joseph on 20/11/2021.
//

#include "../../../public/flow_wm_xlib.hpp"
#include "../../../public/general/input_functions.hpp"
#include <X11/Xatom.h>
#include <xlib/screens/screens.hpp>
#include "../../../public/general/inline_functions.hpp"
#include "../../../../logger/public/logger.hpp"
#include "../../../public/general/masks.hpp"
#include <X11/Xmd.h>

#define _NET_WM_STATE_REMOVE    0
#define _NET_WM_STATE_ADD    1
#define _NET_WM_STATE_TOGGLE    2

#define MWM_HINTS_ELEMENTS 5
#define MWM_DECOR_ALL(x) ((x) & (1L << 0))
#define MWM_DECOR_TITLE(x) ((x) & (1L << 3))
#define MWM_DECOR_BORDER(x) ((x) & (1L << 1))
#define MWM_HINTS_DECOR(x) ((x) & (1L << 1))

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
		auto root = fwm->GetRootWindow();
		auto dpy = fwm->GetDisplay();
		auto config = fwm->GetConfig();
		Client* t, * client = new Client();
		Window trans = None;
		XWindowChanges wc;
		XSetWindowAttributes swa;
		unsigned long mask = 0;

		client->window = window;
		client->position.x = client->old_position.x = wa->x;
		client->position.y = client->old_position.y = wa->y;
		client->position.width = client->old_position.width = wa->width;
		client->position.height = client->old_position.height = wa->height;

		if (CheckAtom(client->window, fwm->GetNetAtom()[NetWMWindowType], fwm->GetNetAtom()[NetWMTypeDesk]) ||
			CheckAtom(client->window, fwm->GetNetAtom()[NetWMWindowType], fwm->GetNetAtom()[NetWMTypeDock]))
		{
			client->is_annoying = true;
		}

		auto offsets = fwm->GetShell()->GetOffsets(client->is_annoying);
		client->frame_offsets = offsets;
		client->border_width = client->is_annoying ? 0 : fwm->GetConfig()->border_size_in_pixels;
		client->old_border_width = client->is_annoying ? 0 : wa->border_width;

		ModernWindowManagerProcessAtom(client);
		client->UpdateSizeHints();
		client->UpdateTitle();


		client->monitor = GetCursorMonitor();
		Monitor* m = client->monitor;
		if (client->position.width > m->mw) client->position.width = m->mw - 2 * client->border_width;
		shapes::CenterRectangleOnPlane(shapes::Rectangle(m->mx, m->my, m->mw, m->mh), &client->position);
		int screen = client->monitor->screen;

		swa.override_redirect = True;
		swa.event_mask = FRAME_MASK;
		mask = CWOverrideRedirect | CWEventMask;
		if (wa->depth == 32)
		{
			mask |= CWColormap | CWBorderPixel | CWBackPixel;
			swa.colormap = XCreateColormap(dpy, root, wa->visual, AllocNone);
			swa.background_pixel = BlackPixel(dpy, screen);
			swa.border_pixel = BlackPixel(dpy, screen);
		}

		client->base = XCreateWindow(
			dpy,
			root,
			client->position.x,
			client->position.y,
			client->position.width,
			client->position.height,
			0,
			wa->depth == 32 ? 32 : DefaultDepth(dpy, screen),
			InputOutput,
			wa->depth == 32 ? wa->visual : DefaultVisual(dpy, screen),
			mask,
			&swa
		);

		wc.border_width = client->border_width;
		XConfigureWindow(dpy, client->base, CWBorderWidth, &wc);
		XSetWindowBorder(dpy, client->base, fwm->GetColorScheme()[SchemeNorm][ColBorder].pixel);

		swa.event_mask = ExposureMask | MOUSE_MASK;

		client->framer = XCreateWindow(
			dpy,
			root,
			client->position.x - offsets.left,
			client->position.y - offsets.top,
			client->position.width + offsets.right,
			client->position.height + offsets.bottom,
			0,
			DefaultDepth(dpy, screen),
			CopyFromParent,
			DefaultVisual(dpy, screen),
			CWEventMask,
			&swa
		);

		client->drawable = XCreatePixmap(
			dpy,
			root,
			client->position.width + offsets.left + offsets.right,
			client->position.height + offsets.top + offsets.bottom,
			DefaultDepth(dpy, screen)
		);

		client->xft_draw = XftDrawCreate(
			dpy,
			client->drawable,
			DefaultVisual(dpy, screen),
			DefaultColormap(dpy, screen)
		);

		client->monitor->clients->AddClient(client);

		swa.event_mask = CLIENT_MASK;
		swa.do_not_propagate_mask = CLIENT_NO_PROPAGATE_MASK;
		XChangeWindowAttributes(dpy, client->window, CWEventMask | CWDontPropagate, &swa);
		XSelectInput(
			dpy,
			window,
			EnterWindowMask | FocusChangeMask | PropertyChangeMask | StructureNotifyMask
		);

		XReparentWindow(dpy, client->window, client->base, offsets.left, offsets.top);
		XReparentWindow(dpy, client->framer, client->base, 0, 0);
		XAddToSaveSet(dpy, client->window);
		XMapWindow(dpy, client->window);
		wc.border_width = 0;
		XConfigureWindow(dpy, window, CWBorderWidth, &wc);

		client->Configure();

		if (CheckAtom(client->window, fwm->GetNetAtom()[NetWMState], fwm->GetNetAtom()[NetWMStateFs]))
		{
			ProcessAtom(client, fwm->GetNetAtom()[NetWMStateFs], 1);
		}

		client->Ban();
		client->UpdateFrame();
		fwm->GetKeyboardManager()->GrabButtons(client, 0);
		if (client->monitor == selected_monitor)
		{
			UnFocus(GetSelectedMonitor()->clients->selected, 1);
		}
		client->monitor->clients->selected = client;
		client->UnBan();
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
		Display* dpy = fwm->GetDisplay();
		Monitor* m = client->monitor;
		if (!destroyed)
		{
			XWindowChanges wc;
			wc.border_width = client->old_border_width;
			XGrabServer(fwm->GetDisplay());
			XSelectInput(dpy, client->base, NoEventMask);
			XUnmapWindow(dpy, client->base);
			XSetErrorHandler([](Display*, XErrorEvent*) -> int
			{ return 0; });

			if (client->framer)
			{
				XftDrawDestroy(client->xft_draw);
				XFreePixmap(dpy, client->drawable);
				XDestroyWindow(dpy, client->framer);
				client->framer = 0;
			}

			XSelectInput(dpy, client->window, CLIENT_MASK & ~(StructureNotifyMask | EnterWindowMask));
			XUngrabButton(dpy, AnyButton, AnyModifier, client->window);
			XReparentWindow(dpy, client->window, fwm->GetRootWindow(), client->position.x, client->position.y);
			XMoveWindow(dpy, client->window, client->position.x, client->position.y);
			wc.border_width = client->old_border_width;
			XConfigureWindow(dpy, client->window, CWBorderWidth, &wc);
			if (selected_monitor->clients->selected == client) Focus(nullptr);
			XDestroyWindow(dpy, client->base);

			XSetErrorHandler(FlowX11ErrorHandler);
			XUngrabServer(dpy);
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
		if (m)
		{
			ReStack(m);
		}
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

		status = XGetWindowProperty(dpy, window, atom, 0L, 64L, False, AnyPropertyType,
			&real, &format, items, &extra, (unsigned char**)&ret);
		if (status != Success)
		{
			*items = 0;
			return nullptr;
		}

		return ret;
	}

	void ScreenManager::ProcessAtom(X11::Client* client, Atom state, int set)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto atom = fwm->GetNetAtom();
		CARD32 data[2];

		data[1] = None;
		if (state == atom[NetWMStateFs])
		{
			Focus(client);
			if ((set == _NET_WM_STATE_ADD || set == _NET_WM_STATE_TOGGLE))
			{
				data[0] = state;
			}
			else if ((set == _NET_WM_STATE_REMOVE ||
				set == _NET_WM_STATE_TOGGLE))
			{
				data[0] = None;
			}
			XChangeProperty(fwm->GetDisplay(), client->window, atom[NetWMState], XA_ATOM, 32,
				PropModeReplace, (unsigned char*)data, 2);
		}
		if (state == atom[NetWMStateModel])
			Focus(client);
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

	void ScreenManager::ModernWindowManagerProcessAtom(X11::Client* client)
	{
		Atom real;
		int format;
		unsigned char* data = nullptr;
		CARD32* hint;
		unsigned long n, extra;
		auto fwm = FlowWindowManagerX11::Get();
		if (XGetWindowProperty(fwm->GetDisplay(), client->window, fwm->GetNetAtom()[NetMWMHints], 0L, 20L, False,
			fwm->GetNetAtom()[NetMWMHints], &real, &format, &n, &extra, &data) == Success && n >= MWM_HINTS_ELEMENTS)
		{
			hint = (CARD32*)data;
			if (MWM_HINTS_DECOR(hint[0]) && !(MWM_DECOR_ALL(hint[2])))
			{
				client->framer = MWM_DECOR_TITLE(hint[2]) ? fwm->GetRootWindow() : (Window)NULL;
				client->border_width = MWM_DECOR_BORDER(hint[2]) ? fwm->GetConfig()->border_size_in_pixels : 0;
			}
		}
		XFree(data);
	}

}