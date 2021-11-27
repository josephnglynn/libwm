//
// Created by joseph on 18/11/2021.
//

#ifndef FLOW_WM_HANDLERS_HPP
#define FLOW_WM_HANDLERS_HPP
#include "flow_wm_xlib.hpp"
#include "public/xlib/client_manager/client_manager.hpp"
#include "../../../../logger/public/logger.hpp"
#include "public/xlib/handler_helpers/handler_helpers.hpp"

#define CLEAN_MASK(mask)         (mask & ~(keyboard_manager->num_lock_mask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))

namespace flow::X11
{

	void FlowWindowManagerX11::OnButtonPress(XEvent& event)
	{

		unsigned int i, click = ClkRootWin;
		XButtonPressedEvent xb = event.xbutton;

		Client* client = client_manager->GetClient(xb.window);
		if (client)
		{
			client->Focus();
			XAllowEvents(display, ReplayPointer, CurrentTime);
			click = ClkClientWin;
		}

		for (i = 0; i < keyboard_manager->key_bindings_client.size(); i++)
		{
			if (click == keyboard_manager->key_bindings_client[i].click
				&& keyboard_manager->key_bindings_client[i].function
				&& keyboard_manager->key_bindings_client[i].button == xb.button
				&& CLEAN_MASK(keyboard_manager->key_bindings_client[i].event_mask) == CLEAN_MASK(xb.state))
			{
				keyboard_manager->key_bindings_client[i].function(keyboard_manager->key_bindings_client[i].arg);
				logger::info("BUTTON CLICK");
			}
		}

	}

	void FlowWindowManagerX11::OnClientMessage(XEvent& event)
	{
		XClientMessageEvent cme = event.xclient;
		Client* client = client_manager->GetClient(cme.window);

		if (!client) return;
		if (cme.message_type == net_atom[NetWMState])
		{
			if (cme.data.l[1] == net_atom[NetWMFullscreen] || cme.data.l[2] == net_atom[NetWMFullscreen])
			{
				client->SetFullScreen((cme.data.l[0] == 1|| (cme.data.l[0] == 2 && !client->full_screen)));
			} else if (cme.message_type == net_atom[NetActiveWindow]) {
				if (!client->is_urgent) {
					client->SetUrgent(1);
				}
			}
		}
	}



	void FlowWindowManagerX11::OnConfigureRequest(XEvent& event)
	{

		Client *c;
		Monitor *m;
		XConfigureRequestEvent ev = event.xconfigurerequest;
		XWindowChanges wc;

		if ((c = client_manager->GetClient(ev.window))) {
			if (ev.value_mask & CWBorderWidth)
				c->border = ev.border_width;
			else if (c->configured)  {
				if (ev.value_mask & CWX) {
					c->old_position.x = c->position.x;
					c->position.x = ev.x;
				}
				if (ev.value_mask & CWY) {
					c->old_position.y = c->position.y;
					c->position.y = ev.y;
				}
				if (ev.value_mask & CWWidth) {
					c->old_position.width = c->position.width;
					c->position.width= ev.width;
				}
				if (ev.value_mask & CWHeight) {
					c->old_position.height = c->position.height;
					c->position.height = ev.height;
				}
				if ((ev.value_mask & (CWX|CWY)) && !(ev.value_mask & (CWWidth|CWHeight))) c->Configure();
			} else {
				c->Configure();
			}
		} else {
			wc.x = ev.x;
			wc.y = ev.y;
			wc.width = ev.width;
			wc.height = ev.height;
			wc.border_width = ev.border_width;
			wc.sibling = ev.above;
			wc.stack_mode = ev.detail;
			XConfigureWindow(display, ev.window, ev.value_mask, &wc);
		}
		XSync(display, False);
	}


	//TODO COME BACK HERE
	void FlowWindowManagerX11::OnConfigureNotify(XEvent& event)
	{
		Monitor *m;
		Client *c;
		XConfigureEvent ev = event.xconfigure;
		int dirty;


		if (ev.window == root_window) {
			dirty = (screen_width!= ev.width || screen_height != ev.height);
			screen_width = ev.width;
			screen_height = ev.height;
			if (dirty) {
				drw->Resize(screen_width, screen_height);
				client_manager->FocusNull();
			}
		}
	}


	void FlowWindowManagerX11::OnDestroyNotify(XEvent& event)
	{
		XDestroyWindowEvent dwe = event.xdestroywindow;
		Client* c = client_manager->GetClient(dwe.window);
		if (c) c->UnManage(1);
	}

	void FlowWindowManagerX11::OnEnterNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::OnExpose(XEvent& event)
	{

	}

	void FlowWindowManagerX11::OnFocusIn(XEvent& event)
	{

	}

	void FlowWindowManagerX11::OnKeyPress(XEvent& event)
	{

	}

	void FlowWindowManagerX11::OnMappingNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::OnMapRequest(XEvent& event)
	{

	}

	void FlowWindowManagerX11::OnMotionNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::OnPropertyNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::OnUnmapNotify(XEvent& event)
	{

	}

}

#endif //FLOW_WM_HANDLERS_HPP
