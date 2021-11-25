//
// Created by joseph on 20/11/2021.
//

#include "public/xlib/client/client.hpp"
#include "public/flow_wm_xlib.hpp"
#include <X11/Xlib.h>
#include <X11/Xatom.h>

flow::X11::Client::Client(Window window) : window(window), visible(true)
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

void flow::X11::Client::SetUrgent(int urgency)
{
	auto fwm = FlowWindowManagerX11::Get();
	XWMHints* wmh;
	is_urgent = urgency;
	if (!(wmh = XGetWMHints(fwm->GetDisplay(), window))) return;
	wmh->flags = urgency ? (wmh->flags | XUrgencyHint) : (wmh->flags & ~XUrgencyHint);
	XSetWMHints(fwm->GetDisplay(), window, wmh);
	XFree(wmh);
}

void flow::X11::Client::FocusClient()
{
	auto fwm = FlowWindowManagerX11::Get();

	if (is_urgent)
	{
		SetUrgent(0);
	}

	fwm->GetKeyboardManager()->GrabButtons(this, 1);
	XSetWindowBorder(fwm->GetDisplay(), window, fwm->GetColorScheme()[SchemeSel][ColBorder].pixel);

}
void flow::X11::Client::SetFocus()
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

int flow::X11::Client::SendEvent(Atom protocol)
{
	auto fwm = FlowWindowManagerX11::Get();
	int n;
	Atom * protocols;
	int exists = 0;
	XEvent ev;

	if (XGetWMProtocols(fwm->GetDisplay(), window, &protocols, &n)) {
		while (!exists && n--) {
			exists = protocols[n] == protocol;
		}
		XFree(protocols);
	}

	if (exists) {
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
