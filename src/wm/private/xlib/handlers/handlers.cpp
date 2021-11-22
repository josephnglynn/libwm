//
// Created by joseph on 18/11/2021.
//

#ifndef FLOW_WM_HANDLERS_HPP
#define FLOW_WM_HANDLERS_HPP
#include "flow_wm_xlib.hpp"
#include "public/xlib/client_manager/client_manager.hpp"
#include "../../../../logger/public/logger.hpp"
#include "public/xlib/handler_helpers/handler_helpers.hpp"

namespace flow::X11
{

	void FlowWindowManagerX11::onKeyPress(XEvent& event)
	{

		char str[256];
		KeySym ks;
		XLookupString(&event.xkey, str, 256, &ks,nullptr);
		logger::success("WE FINALLY GOT A KEY PRESS", event.xkey.keycode , str);

	}

	void FlowWindowManagerX11::onKeyRelease(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onButtonPress(XEvent& event)
	{
		logger::info(client_manager->GetCount());
	}

	void FlowWindowManagerX11::onButtonRelease(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onMotionNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onEnterNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onLeaveNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onFocusIn(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onFocusOut(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onKeymapNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onExpose(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onGraphicsExpose(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onNoExpose(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onVisibilityNotify(XEvent& event)
	{

	}

	//The X server generates this event whenever a client application creates a window by calling XCreateWindow() or XCreateSimpleWindow().
	void FlowWindowManagerX11::onCreateNotify(XEvent& event)
	{
		XCreateWindowEvent cwe = event.xcreatewindow;
		if (client_manager->Exists(cwe.window))
		{ return; }

		auto* client = new Client();
		client->window = cwe.window;
		client_manager->AddClient(client);

		logger::warn("ADDED CLIENT", cwe.window);

	}

	void FlowWindowManagerX11::onDestroyNotify(XEvent& event)
	{
		XDestroyWindowEvent dwe = event.xdestroywindow;
		client_manager->RemoveClient(dwe.window);
		logger::warn("DESTROYED CLIENT", dwe.window);
	}

	void FlowWindowManagerX11::onUnmapNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onMapNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onMapRequest(XEvent& event)
	{
		XMapRequestEvent me = event.xmaprequest;
		XMapWindow(display, me.window);
	}

	void FlowWindowManagerX11::onReparentNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onConfigureNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onConfigureRequest(XEvent& event)
	{

		XConfigureRequestEvent cre = event.xconfigurerequest;
		XWindowChanges window_changes;

		if (client_manager->Exists(cre.window))
		{
			Client* c = client_manager->GetClient(cre.window);
			auto m = screen_manager->GetClientMonitor(c->position);

			int target_width = static_cast<int>( m->width * 0.8);
			int target_height = static_cast<int>(m->height * 0.8);

			if (target_width < cre.width)
			{
				cre.width = target_width;
			}

			if (target_height < cre.height)
			{
				cre.height = target_height;
			}

			c->position = handler_helpers::XConfigureRequestEventToRect(cre);

			shapes::CenterRectangleOnPlane(m->toRectangle(), &c->position);

			window_changes.x = c->position.x;
			window_changes.y = c->position.y;
			window_changes.width = static_cast<int>(c->position.width);
			window_changes.height = static_cast<int>(c->position.height);
		}
		else
		{
			logger::error("HMM, Client doesn't exist", cre.window);
			window_changes.x = cre.x;
			window_changes.y = cre.y;
			window_changes.width = cre.width;
			window_changes.height = cre.height;
		}

		window_changes.stack_mode = cre.detail;
		window_changes.border_width = cre.border_width;
		window_changes.sibling = cre.above;
		XConfigureWindow(display, cre.window, cre.value_mask, &window_changes);
	}

	void FlowWindowManagerX11::onGravityNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onResizeRequest(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onCirculateNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onPropertyNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onSelectionClear(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onSelectionRequest(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onSelectionNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onColormapNotify(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onClientMessage(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onMappingNotify(XEvent& event)
	{
		XMappingEvent ev = event.xmapping;
		XRefreshKeyboardMapping(&ev);
		if (ev.request == MappingKeyboard)
		{
			keyboard_manager->Start(display, root_window);
		}
	}

	void FlowWindowManagerX11::onGenericEvent(XEvent& event)
	{

	}

	void FlowWindowManagerX11::onLastEvent(XEvent& event)
	{

	}

}

#endif //FLOW_WM_HANDLERS_HPP
