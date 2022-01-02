//
// Created by joseph on 20/11/2021.
//


#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "../../../public/xlib/enums/enums.hpp"
#include "../../../public/xlib/client/client.hpp"
#include "../../../public/flow_wm_xlib.hpp"
#include "../../../public/general/inline_functions.hpp"

#include "../../../../logger/public/logger.hpp"

namespace flow::X11
{
	Client::Client(Window window) : window(window), visible(true)
	{
	}

	void Client::SetUrgent(int urgency)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();

		XWMHints* wmh;
		is_urgent = urgency;
		if (!(wmh = XGetWMHints(display, window))) return;
		wmh->flags = urgency ? (wmh->flags | XUrgencyHint) : (wmh->flags & ~XUrgencyHint);
		XSetWMHints(display, window, wmh);
		XFree(wmh);
	}

	void Client::SetFocus()
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();

		if (!never_focus)
		{
			XSetInputFocus(display, window, RevertToPointerRoot, CurrentTime);
			XChangeProperty(
				display,
				fwm->GetRootWindow(),
				fwm->GetNetAtom()[NetActiveWindow],
				XA_WINDOW,
				32,
				PropModeReplace,
				(unsigned char*)&window,
				1
			);
		}

		SendEvent(fwm->GetWmAtom()[WMTakeFocus]);
	}

	int Client::SendEvent(Atom protocol)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();

		int n;
		Atom* protocols;
		int exists = 0;
		XEvent ev;

		if (XGetWMProtocols(display, window, &protocols, &n))
		{
			while (!exists && n--)
			{
				exists = protocols[n] == protocol;
			}
			XFree(protocols);
		}

		if (exists)
		{
			ev.type = ClientMessage;
			ev.xclient.window = window;
			ev.xclient.message_type = fwm->GetWmAtom()[WMProtocols];
			ev.xclient.format = 32;
			ev.xclient.data.l[0] = protocol;
			ev.xclient.data.l[1] = CurrentTime;
			XSendEvent(display, window, False, NoEventMask, &ev);
		}

		return exists;
	}

	void Client::UpdateWindowType()
	{
		auto fwm = FlowWindowManagerX11::Get();
		Atom state = GetAtomProp(fwm->GetNetAtom()[NetWMState]);

		if (state == fwm->GetNetAtom()[NetWMFullscreen])
			SetFullScreen(1);
	}

	Atom Client::GetAtomProp(Atom prop)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();

		int di;
		unsigned long dl;
		unsigned char* p = nullptr;
		Atom da, atom = None;

		if (XGetWindowProperty(display, window, prop, 0L, sizeof atom, False, XA_ATOM,
			&da, &di, &dl, &dl, &p) == Success && p)
		{
			atom = *(Atom*)p;
			XFree(p);
		}
		return atom;
	}

	void Client::SetFullScreen(int fs)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();
		auto cm = fwm->GetClientManager();

		if (fs && !full_screen)
		{
			XChangeProperty(
				display,
				window,
				fwm->GetNetAtom()[NetWMState],
				XA_ATOM,
				32,
				PropModeReplace,
				(unsigned char*)&fwm->GetNetAtom()[NetWMFullscreen],
				1
			);
			full_screen = true;
			old_border_width = border_width;
			border_width = 0;
			cm->UnFrame(this);
			shapes::Rectangle rec_mon = shapes::Rectangle(monitor->mx, monitor->my, monitor->mw, monitor->mh);
			ResizeClient(rec_mon);
			XRaiseWindow(display, window);
		}
		else if (!fs && full_screen)
		{
			XChangeProperty(
				display,
				window,
				fwm->GetNetAtom()[NetWMState],
				XA_ATOM,
				32,
				PropModeReplace,
				(unsigned char*)nullptr,
				0
			);
			full_screen = false;
			border_width = old_border_width;
			position = old_position;
			ResizeClient(position);
			cm->Frame(this);
			fwm->GetScreenManager()->Arrange(monitor);
		}
	}

	void Client::ResizeClient(flow::shapes::Rectangle& detail)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();

		if (detail.width <= frame_offsets.left + frame_offsets.right)
		{
			detail.width = frame_offsets.left + frame_offsets.right + 1;

		}

		if (detail.height <= frame_offsets.top + frame_offsets.bottom)
		{
			detail.height = frame_offsets.top + frame_offsets.bottom + 1;
		}

		XWindowChanges wc;

		old_position = position;
		position = detail;

		XMoveResizeWindow(display, frame, detail.x, detail.y, detail.width, detail.height);
		XMoveResizeWindow(display,
			window,
			frame_offsets.left,
			frame_offsets.top,
			detail.width - (frame_offsets.right + frame_offsets.left),
			detail.height - (frame_offsets.bottom + frame_offsets.top)
		);
		Configure();
		XSync(display, false);
	}

	void Client::ResizeClient(int x, int y, int w, int h)
	{
		auto rect = shapes::Rectangle(x, y, w, h);
		return ResizeClient(rect);
	}

	void Client::Configure()
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();

		configured = true;
		XConfigureEvent ce;

		ce.type = ConfigureNotify;
		ce.display = display;
		ce.event = frame;
		ce.window = frame;
		ce.x = position.x;
		ce.y = position.y;
		ce.width = position.width;
		ce.height = position.height;
		ce.border_width = border_width;
		ce.above = None;
		ce.override_redirect = false;
		XSendEvent(display, frame, false, StructureNotifyMask, (XEvent*)&ce);

		ce.event = window;
		ce.window = window;
		ce.x = frame_offsets.left;
		ce.y = frame_offsets.top;
		ce.width -= (frame_offsets.right + frame_offsets.left);
		ce.height -= (frame_offsets.bottom + frame_offsets.top);
		XSendEvent(display, window, false, StructureNotifyMask, (XEvent*)&ce);
	}

	void Client::UpdateSizeHints()
	{
		long msize;
		XSizeHints size;

		if (!XGetWMNormalHints(FlowWindowManagerX11::Get()->GetDisplay(), window, &size, &msize) || !size.flags)
		{
			size.flags = PSize;
		}

		flags = size.flags;
		if (flags & PBaseSize)
		{
			base_width = size.base_width;
			base_height = size.base_height;
		}
		else if (flags & PMinSize)
		{
			base_width = size.min_width;
			base_height = size.min_height;
		}
		else
			base_width = base_height = 0;
		if (flags & PResizeInc)
		{
			inc_width = size.width_inc;
			inc_height = size.height_inc;
		}
		else
			inc_width = inc_height = 0;
		if (flags & PMaxSize)
		{
			max_width = size.max_width;
			max_height = size.max_height;
		}
		else
			max_width = max_height = 0;
		if (flags & PMinSize)
		{
			min_width = size.min_width;
			min_height = size.min_height;
		}
		else if (flags & PBaseSize)
		{
			min_width = size.base_width;
			min_height = size.base_height;
		}
		else
			min_width = min_height = 0;
	}

	void Client::UpdateWmHints()
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();

		XWMHints* wmh;

		if ((wmh = XGetWMHints(display, window)))
		{
			if (this == fwm->GetClientManager()->selected && wmh->flags & XUrgencyHint)
			{
				wmh->flags &= ~XUrgencyHint;
				XSetWMHints(display, window, wmh);
			}
			else
			{
				is_urgent = (wmh->flags & XUrgencyHint) ? 1 : 0;
			}

			if (wmh->flags & InputHint)
			{
				never_focus = !wmh->input;
			}
			else
			{
				never_focus = false;
			}

			XFree(wmh);
		}

	}

	void Client::SetState(long state)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();
		long data[] = { state, None };

		XChangeProperty(
			display,
			window,
			fwm->GetWmAtom()[WMState],
			fwm->GetWmAtom()[WMState],
			32,
			PropModeReplace,
			(unsigned char*)data,
			2
		);
	}

	int Client::ApplySizeHints(int* x, int* y, int* w, int* h, int interact)
	{
		auto fwm = FlowWindowManagerX11::Get();
		int screen_width = fwm->GetScreenWidth();
		int screen_height = fwm->GetScreenHeight();
		int base_min;
		bool w_flip = false, h_flip = false;


		if (*w < 0)
		{
			*x += *w;
			*w = -*w;
			w_flip = true;
		}

		if (*h < 0)
		{
			*y += *h;
			*h = -*h;
			h_flip = true;
		}


		if (interact)
		{
			if (*x > screen_width)
				*x = screen_width - position.width;
			if (*y > screen_height)
				*y = screen_height - position.height;
			if (*x + *w + 2 * border_width < 0)
				*x = 0;
			if (*y + *h + 2 * border_width < 0)
				*y = 0;
		}
		else
		{
			if (*x >= monitor->wx + monitor->ww)
				*x = monitor->wx + monitor->ww - position.width;
			if (*y >= monitor->wy + monitor->wh)
				*y = monitor->wy + monitor->wh - position.height;
			if (*x + *w + 2 * border_width <= monitor->wx)
				*x = monitor->wx;
			if (*y + *h + 2 * border_width <= monitor->wy)
				*y = monitor->wy;
		}

		base_min = base_width == min_width && base_height == min_height;
		if (!base_min)
		{
			*w -= base_width;
			*h -= base_height;
		}

		if (min_a > 0 && max_a > 0)
		{
			if (max_a < (float)*w / *h)
				*w = *h * max_a + 0.5;
			else if (min_a < (float)*h / *w)
				*h = *w * min_a + 0.5;
		}
		if (base_min)
		{
			*w -= base_width;
			*h -= base_height;
		}

		if (inc_width)
			*w -= *w % inc_width;
		if (inc_height)
			*h -= *h % inc_height;
		/* restore base dimensions */
		*w = Max(*w + base_width, min_width);
		*h = Max(*h + base_height, min_height);
		if (max_width)
			*w = Min(*w, max_width);
		if (max_height)
			*h = Min(*h, max_height);

		return *x != position.x || *y != position.y || *w != static_cast<int>(position.width)
			|| *h != static_cast<int>(position.height);
	}

	void Client::SendMonitor(Monitor* m)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto cm = fwm->GetClientManager();
		if (monitor == m) return;
		cm->UnFocus(this, 1);
		monitor = m;
		cm->Focus(nullptr);
	}

	void Client::UpdateTitle()
	{
		char c_name[256];
		strcpy(c_name, name.data());
		auto fwm = FlowWindowManagerX11::Get();
		if (!fwm->GetTextProp(window, fwm->GetNetAtom()[NetWMName], c_name, sizeof c_name))
		{
			fwm->GetTextProp(window, XA_WM_NAME, c_name, sizeof c_name);
		}
		if (name[0] == '\0')
		{
			strcpy(c_name, "broken");
		}

		name = std::string(c_name);
		logger::success("NAME IS NOW: ", name);

	}

}