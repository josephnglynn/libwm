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

namespace flow::X11
{
	enum
	{
		CurNormal, CurResize, CurMove, CurLast
	};

	enum
	{
		NetSupported, NetWMName, NetWMState, NetWMCheck,
		NetWMFullscreen, NetActiveWindow, NetWMWindowType,
		NetWMWindowTypeDialog, NetClientList, NetLast
	};

	enum
	{
		WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast
	};

	enum
	{
		SchemeNorm, SchemeSel
	};

	enum
	{
		ColFg, ColBg, ColBorder
	};

	enum
	{
		ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
		ClkClientWin, ClkRootWin, ClkLast
	};

	struct DrawableWindow;

	struct Fnt
	{
		Display* dpy;
		unsigned int h;
		XftFont* xfont;
		FcPattern* pattern;
		struct Fnt* next;

		static Fnt* XCreateFont(DrawableWindow* drw, const char* fontname, FcPattern* fontpattern);

	};

	struct DrawableWindow
	{
		unsigned int w, h;
		Display* dpy;
		int screen;
		Window root;
		Drawable drawable;
		GC gc;
		XftColor* scheme;
		Fnt* fonts;

		static DrawableWindow* Create(Display* display, int screen, Window root, unsigned int w, unsigned int h);
		static Fnt* CreateFontSet(DrawableWindow* drw, std::vector<std::string> fonts);
	};

	namespace ColorScheme
	{
		XftColor* ScmCreate(DrawableWindow* drw, const char* colour_names[], size_t colour_count);
		void ClrCreate(DrawableWindow* drw, XftColor* destination, const char* color_name);
	}

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
