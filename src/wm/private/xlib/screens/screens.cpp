//
// Created by joseph on 20/11/2021.
//

#include "../../../public/flow_wm_xlib.hpp"
#include "../../../public/general/input_functions.hpp"
#include <xlib/screens/screens.hpp>
#include "../../../public/general/inline_functions.hpp"


namespace flow
{

	using namespace X11;

	int ScreenManager::UpdateGeom()
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto cm = fwm->GetClientManager();

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
					while ((c = cm->GetFirst()))
					{
						dirty = 1;
						cm->SetFirst(c->next);
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
		free(monitor);
	}

	Monitor* ScreenManager::WindowToMonitor(Window w)
	{
		auto fwm = FlowWindowManagerX11::Get();
		int x, y;
		Client* c;

		if (w == fwm->GetRootWindow() && input_functions::GetRootPointer(&x, &y))
			return RectToMonitor(shapes::Rectangle(x, y, 1, 1));
		if ((c = fwm->GetClientManager()->WindowToClient(w)))
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



	void ScreenManager::ReStack(Monitor* m)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();
		Client* c = fwm->GetClientManager()->selected;
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




}