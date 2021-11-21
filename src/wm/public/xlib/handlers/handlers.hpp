//
// Created by joseph on 21/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_HANDLERS_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_HANDLERS_HPP
#include <X11/Xlib.h>

namespace flow::X11::handlers
{

	void onKeyPress(XEvent& event);

	void onKeyRelease(XEvent& event);

	void onButtonPress(XEvent& event);

	void onButtonRelease(XEvent& event);

	void onMotionNotify(XEvent& event);

	void onEnterNotify(XEvent& event);

	void onLeaveNotify(XEvent& event);

	void onFocusIn(XEvent& event);

	void onFocusOut(XEvent& event);

	void onKeymapNotify(XEvent& event);

	void onExpose(XEvent& event);

	void onGraphicsExpose(XEvent& event);

	void onNoExpose(XEvent& event);

	void onVisibilityNotify(XEvent& event);

	//The X server generates this event whenever a client application creates a window by calling XCreateWindow() or XCreateSimpleWindow().
	void onCreateNotify(XEvent& event);

	void onDestroyNotify(XEvent& event);

	void onUnmapNotify(XEvent& event);

	void onMapNotify(XEvent& event);

	void onMapRequest(XEvent& event);

	void onReparentNotify(XEvent& event);

	void onConfigureNotify(XEvent& event);

	void onConfigureRequest(XEvent& event);

	void onGravityNotify(XEvent& event);

	void onResizeRequest(XEvent& event);

	void onCirculateNotify(XEvent& event);

	void onPropertyNotify(XEvent& event);

	void onSelectionClear(XEvent& event);

	void onSelectionRequest(XEvent& event);

	void onSelectionNotify(XEvent& event);

	void onColormapNotify(XEvent& event);

	void onClientMessage(XEvent& event);

	void onMappingNotify(XEvent& event);

	void onGenericEvent(XEvent& event);

	void onLastEvent(XEvent& event);
}

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_HANDLERS_HPP
