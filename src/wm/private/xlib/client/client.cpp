//
// Created by joseph on 20/11/2021.
//


#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "../../../public/xlib/enums/enums.hpp"
#include "../../../public/xlib/client/client.hpp"
#include "../../../public/flow_wm_xlib.hpp"

namespace flow::X11
{
	Client::Client(Window window) : window(window), visible(true)
	{
		XGetGeometry(
			FlowWindowManagerX11::Get()->GetDisplay(),
			window,
			nullptr,
			&position.x,
			&position.y,
			&position.width,
			&position.height,
			&border,
			&depth
		);
	}

	void Client::SetUrgent(int urgency)
	{
		auto fwm = FlowWindowManagerX11::Get();
		XWMHints* wmh;
		is_urgent = urgency;
		if (!(wmh = XGetWMHints(fwm->GetDisplay(), window))) return;
		wmh->flags = urgency ? (wmh->flags | XUrgencyHint) : (wmh->flags & ~XUrgencyHint);
		XSetWMHints(fwm->GetDisplay(), window, wmh);
		XFree(wmh);
	}

	void Client::Focus()
	{
		auto fwm = FlowWindowManagerX11::Get();

		if (is_urgent)
		{
			SetUrgent(0);
		}

		GrabButtons(1);
		XSetWindowBorder(fwm->GetDisplay(), window, fwm->GetColorScheme()[SchemeSel][ColBorder].pixel);

	}
	void Client::SetFocus()
	{
		auto fwm = FlowWindowManagerX11::Get();
		if (!never_focus)
		{
			XSetInputFocus(fwm->GetDisplay(), window, RevertToPointerRoot, CurrentTime);
			XChangeProperty(
				fwm->GetDisplay(),
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
		int n;
		Atom* protocols;
		int exists = 0;
		XEvent ev;

		if (XGetWMProtocols(fwm->GetDisplay(), window, &protocols, &n))
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
			XSendEvent(fwm->GetDisplay(), window, False, NoEventMask, &ev);
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
		int di;
		unsigned long dl;
		unsigned char* p = nullptr;
		Atom da, atom = None;

		if (XGetWindowProperty(fwm->GetDisplay(), window, prop, 0L, sizeof atom, False, XA_ATOM,
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
		if (fs && !full_screen)
		{
			XChangeProperty(fwm->GetDisplay(), window, fwm->GetNetAtom()[NetWMState], XA_ATOM, 32,
				PropModeReplace, (unsigned char*)&fwm->GetNetAtom()[NetWMFullscreen], 1);
			full_screen = true;
			border = 0;
			ResizeClient(position);
			XRaiseWindow(fwm->GetDisplay(), window);
		}
		else if (!fs && full_screen)
		{
			XChangeProperty(fwm->GetDisplay(), window, fwm->GetNetAtom()[NetWMState], XA_ATOM, 32,
				PropModeReplace, (unsigned char*)0, 0);
			full_screen = false;
			border = 0;//TODO
			position = old_position;
			ResizeClient(position);
		}
	}

	void Client::ResizeClient(flow::shapes::Rectangle& detail)
	{
		auto fwm = FlowWindowManagerX11::Get();
		XWindowChanges wc;

		old_position = position;
		position = detail;

		wc.x = detail.x;
		wc.y = detail.y;
		wc.width = static_cast<int>(detail.width);
		wc.height = static_cast<int>(detail.height);
		wc.border_width = static_cast<int>(border);

		XConfigureWindow(fwm->GetDisplay(), window, CWX | CWY | CWWidth | CWHeight | CWBorderWidth, &wc);
		Configure();
		XSync(fwm->GetDisplay(), False);

	}

	void Client::ResizeClient(int x, int y, int w, int h)
	{
		auto rect = shapes::Rectangle(x, y, w, h);
		return ResizeClient(rect);
	}

	void Client::Configure()
	{
		auto fwm = FlowWindowManagerX11::Get();
		configured = true;
		XConfigureEvent ce;

		ce.type = ConfigureNotify;
		ce.display = fwm->GetDisplay();
		ce.event = window;
		ce.window = window;
		ce.x = position.x;
		ce.y = position.y;
		ce.width = static_cast<int>(position.width);
		ce.height = static_cast<int>(position.height);
		ce.border_width = static_cast<int>(border);
		ce.above = None;
		ce.override_redirect = False;
		XSendEvent(fwm->GetDisplay(), window, False, StructureNotifyMask, (XEvent*)&ce);
	}

	void Client::UpdateSizeHints()
	{
//TODO
	}

	void Client::UpdateWmHints()
	{
		auto fwm = FlowWindowManagerX11::Get();
		XWMHints* wmh;

		if ((wmh = XGetWMHints(fwm->GetDisplay(), window)))
		{
			if (this == fwm->GetClientManager()->selected_window && wmh->flags & XUrgencyHint)
			{
				wmh->flags &= ~XUrgencyHint;
				XSetWMHints(fwm->GetDisplay(), window, wmh);
			}
			else
				is_urgent = (wmh->flags & XUrgencyHint) ? 1 : 0;
			if (wmh->flags & InputHint)
				never_focus = !wmh->input;
			else
				never_focus = 0;
			XFree(wmh);
		}

	}

	void Client::SetState(long state)
	{
		auto fwm = FlowWindowManagerX11::Get();
		long data[] = { state, None };

		XChangeProperty(fwm->GetDisplay(),
			window,
			fwm->GetWmAtom()[WMState],
			fwm->GetWmAtom()[WMState],
			32,
			PropModeReplace,
			(unsigned char*)data,
			2
		);
	}

	void Client::GrabButtons(int focused)
	{
		auto fwm = X11::FlowWindowManagerX11::Get();
		fwm->GetKeyboardManager()->UpdateNumLockMask(fwm->GetDisplay());
		unsigned int i, j;
		unsigned int modifiers[] = {
			0,
			LockMask,
			fwm->GetKeyboardManager()->num_lock_mask,
			fwm->GetKeyboardManager()->num_lock_mask | LockMask
		};
		XUngrabButton(fwm->GetDisplay(), AnyButton, AnyModifier, window);
		if (!focused)
		{
			XGrabButton(
				fwm->GetDisplay(),
				AnyButton,
				AnyModifier,
				window,
				False,
				ButtonPressMask | ButtonReleaseMask,
				GrabModeSync,
				GrabModeSync,
				None,
				None
			);
		}
		for (i = 0; i < fwm->GetKeyboardManager()->key_bindings_client.size(); i++)
		{
			if (fwm->GetKeyboardManager()->key_bindings_client[i].click == ClkClientWin)
			{
				for (j = 0; j < sizeof modifiers / sizeof modifiers[0]; j++)
					XGrabButton(
						fwm->GetDisplay(), fwm->GetKeyboardManager()->key_bindings_client[i].button,
						fwm->GetKeyboardManager()->key_bindings_client[i].event_mask | modifiers[j],
						window, False, ButtonPressMask | ButtonReleaseMask,
						GrabModeAsync, GrabModeSync, None, None
					);
			}
		}

	}


}