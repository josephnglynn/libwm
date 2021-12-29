//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_FLOW_WM_XLIB_HPP
#define FLOW_WM_FLOW_WM_XLIB_HPP
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <array>
#include "general/config.hpp"
#include "xlib/client_manager/client_manager.hpp"
#include "xlib/screens/screens.hpp"
#include "xlib/keyboard_manager/keyboard_manager.hpp"
#include "xlib/cursor/cursor.hpp"
#include "xlib/drawable_window/drawable_window.hpp"
#include "xlib/enums/enums.hpp"
#include "xlib/shell/shell.hpp"

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
		void Detach();
		void HandleEvent(XEvent& event);

		[[nodiscard]]    inline Display* GetDisplay()
		{
			return display;
		}

		[[nodiscard]]    inline Window GetRootWindow() const
		{
			return root_window;
		}

		[[nodiscard]]    inline Atom* GetNetAtom()
		{
			return net_atom;
		}

		[[nodiscard]]    inline XftColor** GetColorScheme()
		{
			return color_scheme;
		}

		[[nodiscard]]    inline KeyboardManager* GetKeyboardManager()
		{
			return keyboard_manager;
		}

		[[nodiscard]]    inline ScreenManager* GetScreenManager()
		{
			return screen_manager;
		}

		[[nodiscard]]    inline Atom* GetWmAtom()
		{
			return wm_atom;
		};

		[[nodiscard]]    inline Cur** GetCursor()
		{
			return cursor;
		}

		[[nodiscard]]    inline int GetScreenWidth() const
		{
			return screen_width;
		}

		[[nodiscard]]    inline int GetScreenHeight() const
		{
			return screen_height;
		}

		[[nodiscard]]    inline Config* GetConfig()
		{
			return config;
		}

		[[nodiscard]]    inline Shell* GetShell()
		{
			return shell;
		}

		[[nodiscard]]    inline Atom GetUFT8String()
		{
			return utf8string;
		}

		[[nodiscard]]    inline Window GetBase()
		{
			return base;
		}

		[[nodiscard]]    inline ClientManager* GetClientManager()
		{
			return client_manager;
		}

		int GetTextProp(Window w, Atom atom, char* text, unsigned int size);

		bool detached = false;
	private:
		Atom utf8string;
		ScreenManager* screen_manager;
		KeyboardManager* keyboard_manager;
		ClientManager* client_manager;
		Config* config;
		Display* display = nullptr;
		Window root_window;
		bool quit = false;
		Atom wm_atom[WMLast], net_atom[NetLast];
		Cur* cursor[CurLast];
		DrawableWindow* drw;
		XftColor** color_scheme;
		Window wm_check_window;
		Shell* shell;
		int screen;
		int screen_width;
		int screen_height;
		std::vector<Window> suicide_list;
		Window base;

		void Scan();
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
		void OnMotionNotify(XEvent& event) const;
		void OnPropertyNotify(XEvent& event);
		void OnUnmapNotify(XEvent& event);

	};
}

#endif //FLOW_WM_FLOW_WM_XLIB_HPP
