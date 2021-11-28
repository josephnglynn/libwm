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
		int screen;
		int screen_width;
		int screen_height;

		void Scan();
		int GetTextProp(Window w, Atom atom, char *text, unsigned int size);
		void UpdateStatus();

		static FlowWindowManagerX11* instance;

		void OnButtonPress(XEvent& event);
		void OnClientMessage(XEvent& event);
		void OnConfigureRequest(XEvent& event);
		void OnConfigureNotify(XEvent& event);
		void OnDestroyNotify(XEvent& event);
		void OnEnterNotify(XEvent& event);
		void OnExpose(XEvent& event);
		void OnFocusIn(XEvent& event);
		void OnKeyPress(XEvent& event);
		void OnMappingNotify(XEvent& event);
		void OnMapRequest(XEvent& event);
		void OnMotionNotify(XEvent& event);
		void OnPropertyNotify(XEvent& event);
		void OnUnmapNotify(XEvent& event);


	};
}

#endif //FLOW_WM_FLOW_WM_XLIB_HPP
