//
// Created by joseph on 18/11/2021.
//

#ifndef FLOW_WM_HANDLERS_HPP
#define FLOW_WM_HANDLERS_HPP
#include <X11/Xatom.h>
#include "../../../public/flow_wm_xlib.hpp"
#include "../../../../logger/public/logger.hpp"
#include "../../../public/general/masks.hpp"
#include "../../../public/general/input_functions.hpp"

namespace flow::X11
{

	void FlowWindowManagerX11::OnButtonPress(XEvent& event)
	{
		unsigned int i, click = ClkRootWin;
		XButtonPressedEvent xb = event.xbutton;
		Client* client;
		Monitor* m;
		Monitor* sel_mon = screen_manager->GetSelectedMonitor();

		if ((m = screen_manager->WindowToMonitor(xb.window)) && m != sel_mon)
		{
			if (sel_mon->clients->selected) screen_manager->UnFocus(sel_mon->clients->selected, 1);
			screen_manager->SetSelectedMonitor(m);
			screen_manager->Focus(nullptr);
		}

		if ((client = screen_manager->WindowToClient(xb.window)))
		{
			screen_manager->Focus(client);
			screen_manager->ReStack(sel_mon);
			XAllowEvents(display, ReplayPointer, CurrentTime);
			XSync(display, false);
			click = ClkClientWin;
		}

		for (i = 0; i < keyboard_manager->key_bindings_client.size(); i++)
		{
			if (click == keyboard_manager->key_bindings_client[i].click
				&& keyboard_manager->key_bindings_client[i].function
				&& keyboard_manager->key_bindings_client[i].button == xb.button
				&& CLEAN_MASK(keyboard_manager->key_bindings_client[i].event_mask) == CLEAN_MASK(xb.state))
			{
				logger::info("BUTTON_PRESSED: ", keyboard_manager->key_bindings_client[i]);
				keyboard_manager->key_bindings_client[i].function(keyboard_manager->key_bindings_client[i].arg);
			}
		}

	}

	void FlowWindowManagerX11::OnClientMessage(XEvent& event)
	{
		XClientMessageEvent cme = event.xclient;
		Client* client = screen_manager->WindowToClient(cme.window);

		if (!client) return;
		if (cme.message_type == net_atom[NetWMState])
		{
			if (static_cast<Atom>(cme.data.l[1]) == net_atom[NetWMFullscreen]
				|| static_cast<Atom>(cme.data.l[2]) == net_atom[NetWMFullscreen])
			{
				client->SetFullScreen((cme.data.l[0] == 1 || (cme.data.l[0] == 2 && !client->full_screen)));
			}
			else if (cme.message_type == net_atom[NetActiveWindow])
			{
				if (client != screen_manager->GetSelectedMonitor()->clients->selected && !client->is_urgent)
				{
					client->SetUrgent(1);
				}
			}
		}
	}

	void FlowWindowManagerX11::OnConfigureRequest(XEvent& event)
	{

		Client* c;
		Monitor* m;
		XConfigureRequestEvent ev = event.xconfigurerequest;
		XWindowChanges wc;

		if ((c = screen_manager->WindowToClient(ev.window)))
		{
			if (ev.value_mask & CWBorderWidth)
				c->border_width = ev.border_width;
			else if (c->configured)
			{
				m = c->monitor;
				if (ev.value_mask & CWX)
				{
					c->old_position.x = c->position.x;
					c->position.x = m->mx + ev.x;
				}
				if (ev.value_mask & CWY)
				{
					c->old_position.y = c->position.y;
					c->position.y = m->my + ev.y;
				}
				if (ev.value_mask & CWWidth)
				{
					c->old_position.width = c->position.width;
					c->position.width = ev.width;
				}
				if (ev.value_mask & CWHeight)
				{
					c->old_position.height = c->position.height;
					c->position.height = ev.height;
				}

				if ((c->position.x + c->position.width) > m->mx + m->mw)
					c->position.x = m->mx + (m->mw / 2 - ((c->position.width + 2 * c->border_width) / 2));
				if ((c->position.y + static_cast<int>(c->position.height)) > m->my + m->mh)
					c->position.y = m->my + (m->mh / 2 - ((c->position.height + 2 * c->border_width) / 2));

				if ((ev.value_mask & (CWX | CWY)) && !(ev.value_mask & (CWWidth | CWHeight))) c->Configure();
				if (c->visible)
					XMoveResizeWindow(
						FlowWindowManagerX11::Get()->GetDisplay(),
						c->window,
						c->position.x,
						c->position.y,
						c->position.width,
						c->position.height
					);
			}
			else
			{
				c->Configure();
			}
		}
		else
		{
			wc.x = ev.x;
			wc.y = ev.y;
			wc.width = ev.width;
			wc.height = ev.height;
			wc.border_width = ev.border_width;
			wc.sibling = ev.above;
			wc.stack_mode = ev.detail;
			XConfigureWindow(display, ev.window, ev.value_mask, &wc);
		}
		XSync(display, false);
	}

	//TODO COME BACK HERE
	void FlowWindowManagerX11::OnConfigureNotify(XEvent& event)
	{
		Monitor* m;
		Client* c;
		XConfigureEvent ev = event.xconfigure;
		int dirty;

		if (ev.window == root_window)
		{
			dirty = (screen_width != ev.width || screen_height != ev.height);
			screen_width = ev.width;
			screen_height = ev.height;
			if (screen_manager->UpdateGeom() || dirty)
			{
				drw->Resize(screen_width, screen_height);
				for (m = screen_manager->GetMons(); m; m = m->next)
				{
					for (c = m->clients->GetFirst(); c; c = c->next)
					{
						if (c->full_screen) c->ResizeClient(m->mx, m->my, m->mw, m->mh);
					}
				}
				screen_manager->Focus(nullptr);
			}
		}
	}

	void FlowWindowManagerX11::OnDestroyNotify(XEvent& event)
	{
		XDestroyWindowEvent dwe = event.xdestroywindow;
		Client* c;
		if ((c = screen_manager->WindowToClient(dwe.window)))
		{
			screen_manager->UnManage(c, 1);
		}
	}

	void FlowWindowManagerX11::OnEnterNotify(XEvent& event)
	{
		Client* c;
		Monitor* m;
		XCrossingEvent ce = event.xcrossing;

		if ((ce.mode != NotifyNormal || ce.detail == NotifyInferior) && ce.window != root_window) return;
		c = screen_manager->WindowToClient(ce.window);
		m = c ? c->monitor : screen_manager->WindowToMonitor(ce.window);

		Monitor* selected_monitor = screen_manager->GetSelectedMonitor();
		if (m != selected_monitor)
		{
			if (selected_monitor->clients->selected)
			{
				screen_manager->UnFocus(selected_monitor->clients->selected, 1);
			}

			screen_manager->SetSelectedMonitor(m);
		}
		else if (!c || c == selected_monitor->clients->selected)
		{
			return;
		}

		screen_manager->Focus(c);
	}

	void FlowWindowManagerX11::OnExpose(XEvent&)
	{
	}

	void FlowWindowManagerX11::OnFocusIn(XEvent& event)
	{
		XFocusChangeEvent fce = event.xfocus;
		if (screen_manager->GetSelectedMonitor()->clients->selected
			&& fce.window != screen_manager->GetSelectedMonitor()->clients->selected->window)
		{
			screen_manager->GetSelectedMonitor()->clients->selected->SetFocus();
		}
	}

	void FlowWindowManagerX11::OnKeyPress(XEvent& event)
	{
		logger::info("KEY PRESSED YAY");
		unsigned int i;
		KeySym keysym;
		XKeyEvent ev = event.xkey;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
		keysym = XKeycodeToKeysym(display, (KeyCode)ev.keycode, 0);
#pragma clang diagnostic pop

		logger::success(keysym, "AND MASK", CLEAN_MASK(ev.state));
		for (i = 0; i < keyboard_manager->key_bindings_root.size(); i++)
		{
			if (keysym == keyboard_manager->key_bindings_root[i].key
				&& CLEAN_MASK(keyboard_manager->key_bindings_root[i].mod_key) == CLEAN_MASK(ev.state)
				&& keyboard_manager->key_bindings_root[i].function)
			{
				keyboard_manager->key_bindings_root[i].function(keyboard_manager->key_bindings_root[i].arg);
			}
		}
	}

	void FlowWindowManagerX11::OnMappingNotify(XEvent& event)
	{
		XMappingEvent me = event.xmapping;

		XRefreshKeyboardMapping(&me);
		if (me.request == MappingKeyboard)
		{
			keyboard_manager->Start(display, root_window);
		}
	}

	void FlowWindowManagerX11::OnMapRequest(XEvent& event)
	{
		static XWindowAttributes wa;
		XMapRequestEvent mre = event.xmaprequest;

		if (!XGetWindowAttributes(display, mre.window, &wa))
			return;
		if (wa.override_redirect)
			return;
		if (!screen_manager->DontTouchWindow(mre.window))
		{
			screen_manager->Manage(mre.window, &wa);
		}
	}

	void FlowWindowManagerX11::OnMotionNotify(XEvent& event)
	{
		XMotionEvent me = event.xmotion;
		if (me.window != root_window) return;
	}

	void FlowWindowManagerX11::OnPropertyNotify(XEvent& event)
	{
		Client* c;
		XPropertyEvent pe = event.xproperty;
		auto fwm = FlowWindowManagerX11::Get();
		if ((pe.window == root_window) && (pe.atom == XA_WM_NAME))
		{
			UpdateStatus();
		}
		else if (pe.state == PropertyDelete)
		{
			return;
		}
		else if ((c = screen_manager->WindowToClient(pe.window)))
		{
			switch (pe.atom)
			{
			case XA_WM_TRANSIENT_FOR:
				break;
			case XA_WM_NORMAL_HINTS:
				c->UpdateWindowType();
				break;
			case XA_WM_HINTS:
				c->UpdateWindowType();
				break;
			case XA_WM_NAME:
				c->UpdateTitle();
				break;
			default:
				if (fwm->GetNetAtom()[NetWMName])
				{
					c->UpdateTitle();
				}
				break;
			}
			if (pe.atom == net_atom[NetWMWindowType]) c->UpdateWindowType();
		}
	}

	void FlowWindowManagerX11::OnUnmapNotify(XEvent& event)
	{
		Client* c;
		XUnmapEvent ume = event.xunmap;

		for (unsigned int i = 0; i < suicide_list.size(); ++i)
		{
			if (suicide_list[i] == ume.window)
			{
				suicide_list.erase(suicide_list.begin() + i);
				return;
			}
		}

		if ((c = screen_manager->WindowToClient(ume.window)))
		{
			if (ume.send_event)
			{
				c->SetState(WithdrawnState);
			}
			else
			{
				screen_manager->UnManage(c, 0);
			}
		}
	}

}

#endif //FLOW_WM_HANDLERS_HPP
