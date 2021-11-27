//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_FLOW_WM_XLIB_HPP
#define FLOW_WM_FLOW_WM_XLIB_HPP
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include "xlib/client_manager/client_manager.hpp"
#include "xlib/screens/screens.hpp"
#include "general/config.hpp"
#include "xlib/keyboard_manager/keyboard_manager.hpp"
#include <array>
#include "xlib/cursor/cursor.hpp"
#include "xlib/drawable_window/drawable_window.hpp"
#include "xlib/enums/enums.hpp"

namespace flow::X11
{


	int FlowX11ErrorHandler(Display* display, XErrorEvent* event);

	class FlowWindowManagerX11
	{
	public:
		~FlowWindowManagerX11();
		static FlowWindowManagerX11* Init(Config* config);
		static FlowWindowManagerX11* Get();

		void SetConfig(Config* c);
		void Start();

		Display* GetDisplay();
		Window GetRootWindow();
		Atom* GetNetAtom();
		XftColor** GetColorScheme();
		KeyboardManager* GetKeyboardManager();
		Atom* GetWmAtom();
		ClientManager* GetClientManager();
	private:
		ScreenManager* screen_manager;
		ClientManager* client_manager;
		KeyboardManager* keyboard_manager;
		Config* config;
		Display* display = nullptr;
		Window root_window;
		bool quit = false;
		Atom wm_atom[WMLast], net_atom[NetLast];
		Cur* cursor[CurLast];
		DrawableWindow* drw;
		XftColor** color_scheme;
		Window wm_check_window;

		void Scan();

		static FlowWindowManagerX11* instance;

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
	};
}

#endif //FLOW_WM_FLOW_WM_XLIB_HPP
