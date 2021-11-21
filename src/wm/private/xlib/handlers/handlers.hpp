//
// Created by joseph on 18/11/2021.
//

#ifndef FLOW_WM_HANDLERS_HPP
#define FLOW_WM_HANDLERS_HPP
#include <iostream>
#include "../client_manager/client_manager.hpp"
#include "handler_helpers.hpp"
#include "../logger/public/logger.hpp"

namespace flow::X11::handlers
{

	void onKeyPress(XEvent& event)
	{

	}

	void onKeyRelease(XEvent& event)
	{

	}

	void onButtonPress(XEvent& event)
	{
		logger::info(ClientManager::Get()->GetCount());
	}

	void onButtonRelease(XEvent& event)
	{

	}

	void onMotionNotify(XEvent& event)
	{

	}

	void onEnterNotify(XEvent& event)
	{

	}

	void onLeaveNotify(XEvent& event)
	{

	}

	void onFocusIn(XEvent& event)
	{

	}

	void onFocusOut(XEvent& event)
	{

	}

	void onKeymapNotify(XEvent& event)
	{

	}

	void onExpose(XEvent& event)
	{

	}

	void onGraphicsExpose(XEvent& event)
	{

	}

	void onNoExpose(XEvent& event)
	{

	}

	void onVisibilityNotify(XEvent& event)
	{

	}

	//The X server generates this event whenever a client application creates a window by calling XCreateWindow() or XCreateSimpleWindow().
	void onCreateNotify(XEvent& event)
	{
		XCreateWindowEvent cwe = event.xcreatewindow;
		ClientManager* cm = ClientManager::Get();
		if (cm->Exists(cwe.window))
		{ return; }

		auto* client = new Client();
		client->window = cwe.window;
		cm->AddClient(client);

		logger::warn("ADDED CLIENT", cwe.window);

	}

	void onDestroyNotify(XEvent& event)
	{
		XDestroyWindowEvent dwe = event.xdestroywindow;
		ClientManager::Get()->RemoveClient(dwe.window);
		logger::warn("DESTROYED CLIENT", dwe.window);
	}

	void onUnmapNotify(XEvent& event)
	{

	}

	void onMapNotify(XEvent& event)
	{

	}

	void onMapRequest(XEvent& event)
	{
		XMapRequestEvent me = event.xmaprequest;
		XMapWindow(FlowWindowManagerX11::Get()->GetDisplay(), me.window);
	}

	void onReparentNotify(XEvent& event)
	{

	}

	void onConfigureNotify(XEvent& event)
	{

	}

	void onConfigureRequest(XEvent& event)
	{
		Display* display = FlowWindowManagerX11::Get()->GetDisplay();
		XConfigureRequestEvent cre = event.xconfigurerequest;
		XWindowChanges window_changes;

		if (ClientManager::Get()->Exists(cre.window))
		{
			Client* c = ClientManager::Get()->GetClient(cre.window);

			handler_helpers::CenterChild(display, c);
			window_changes.x = c->shape.x;
			window_changes.y = c->shape.y;
			window_changes.width = static_cast<int>(c->shape.width);
			window_changes.height = static_cast<int>(c->shape.height);
		}
		else
		{
			logger::error("Doesn't exist", cre.window);
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

	void onGravityNotify(XEvent& event)
	{

	}

	void onResizeRequest(XEvent& event)
	{

	}

	void onCirculateNotify(XEvent& event)
	{

	}

	void onPropertyNotify(XEvent& event)
	{

	}

	void onSelectionClear(XEvent& event)
	{

	}

	void onSelectionRequest(XEvent& event)
	{

	}

	void onSelectionNotify(XEvent& event)
	{

	}

	void onColormapNotify(XEvent& event)
	{

	}

	void onClientMessage(XEvent& event)
	{

	}

	void onMappingNotify(XEvent& event)
	{

	}

	void onGenericEvent(XEvent& event)
	{

	}

	void onLastEvent(XEvent& event)
	{

	}
}

#endif //FLOW_WM_HANDLERS_HPP
