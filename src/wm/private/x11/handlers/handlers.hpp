//
// Created by joseph on 18/11/2021.
//

#ifndef FLOW_WM_HANDLERS_HPP
#define FLOW_WM_HANDLERS_HPP
#include <iostream>

namespace flow::handlers {


	void onKeyPress (XEvent& event) {

	}

	void onKeyRelease (XEvent& event) {

	}


	void onButtonPress(XEvent& event) {

	}

	void onButtonRelease (XEvent& event) {

	}

	void onMotionNotify (XEvent& event) {

	}

	void onEnterNotify (XEvent& event) {

	}


	void onLeaveNotify (XEvent& event) {

	}

	void onFocusIn (XEvent& event) {

	}

	void onFocusOut (XEvent& event) {

	}

	void onKeymapNotify (XEvent& event) {

	}

	void onExpose (XEvent& event) {

	}

	void onGraphicsExpose (XEvent& event) {

	}

	void onNoExpose (XEvent& event) {

	}

	void onVisibilityNotify (XEvent& event) {

	}

	//The X server generates this event whenever a client application creates a window by calling XCreateWindow() or XCreateSimpleWindow().
	void onCreateNotify(XEvent& event) {

	}


	void onDestroyNotify (XEvent& event) {

	}


	void onUnmapNotify (XEvent& event) {

	}


	void onMapNotify(XEvent& event) {

	}


	void onMapRequest (XEvent& event, FlowWindowManagerX11& flow_window_manager) {
		XMapRequestEvent me = event.xmaprequest;
//ADD TO CLIENT HERE
		XMapWindow(flow_window_manager.getDisplay(), me.window);
	}


	void onReparentNotify (XEvent& event) {

	}


	void onConfigureNotify (XEvent& event) {

	}

	void onConfigureRequest (XEvent& event, FlowWindowManagerX11& flow_window_manager) {
		XConfigureRequestEvent cre = event.xconfigurerequest;
		//TODO IF WINDOW DOESN'T ALREADY EXIST
		XWindowChanges window_changes; //TODO WINDOW CHANGES
		window_changes.x = cre.x;
		window_changes.y = cre.y;
		window_changes.width = cre.width;
		window_changes.height = cre.height;
		window_changes.stack_mode = cre.detail;
		window_changes.border_width = cre.border_width;
		window_changes.sibling = cre.above;
		XConfigureWindow(flow_window_manager.getDisplay(), cre.window, cre.value_mask, &window_changes);
	}

	void onGravityNotify (XEvent& event) {

	}

	void onResizeRequest (XEvent& event) {

	}

	void onCirculateNotify (XEvent& event) {

	}

	void onPropertyNotify (XEvent& event) {

	}

	void onSelectionClear (XEvent& event) {

	}

	void onSelectionRequest (XEvent& event) {

	}

	void onSelectionNotify (XEvent& event) {

	}

	void onColormapNotify (XEvent& event) {

	}


	void onClientMessage (XEvent& event) {

	}

	void onMappingNotify (XEvent& event) {

	}

	void onGenericEvent (XEvent& event) {

	}

	void onLastEvent (XEvent& event) {

	}
}

#endif //FLOW_WM_HANDLERS_HPP
