#include "flow_wm_xlib.hpp"
#include "../../../logger/public/logger.hpp"
#include <X11/Xlib.h>
#include <string>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include "xlib/color_scheme/color_scheme.hpp"
#include "xlib/enums/enums.hpp"

#ifndef FLOW_WM_VERSION
#define FLOW_WM_VERSION "unofficial-build"
#endif

namespace flow::X11
{

	FlowWindowManagerX11* FlowWindowManagerX11::instance;

	void FlowWindowManagerX11::Start()
	{

		while (!quit)
		{

			XEvent event;
			XNextEvent(display, &event);
			if (event.type == MotionNotify) continue;

			switch (event.type)
			{
			case KeyPress:
				OnKeyPress(event);
				break;
			case ButtonPress:
				OnButtonPress(event);
				break;
			case MotionNotify:
				OnMotionNotify(event);
				break;
			case EnterNotify:
				OnEnterNotify(event);
				break;
			case FocusIn:
				OnFocusIn(event);
				break;
			case Expose:
				OnExpose(event);
				break;
			case DestroyNotify:
				OnDestroyNotify(event);
				break;
			case UnmapNotify:
				OnUnmapNotify(event);
				break;
			case MapRequest:
				OnMapRequest(event);
				break;
			case ConfigureNotify:
				OnConfigureNotify(event);
				break;
			case ConfigureRequest:
				OnConfigureRequest(event);
				break;
			case PropertyNotify:
				OnPropertyNotify(event);
				break;
			case ClientMessage:
				OnClientMessage(event);
				break;
			case MappingNotify:
				OnMappingNotify(event);
				break;
			default:
				logger::warn("WHATEVER EVENT ", std::to_string(event.type), " IS, WE MUST HAVE IGNORED IT");
				break;
			}
		}
	}

	int FlowX11ErrorHandler(Display*, XErrorEvent* event)
	{
		logger::error(
			"An X11 Error Occurred! Don't worry though, it will not stop execution\nError code: ",
			std::to_string(event->error_code),
			"\nType: " + std::to_string(event->type)
		);
		return 0;
	}

	FlowWindowManagerX11::~FlowWindowManagerX11()
	{
		XCloseDisplay(display);
	}

	FlowWindowManagerX11* FlowWindowManagerX11::Init(Config* config)
	{
		if (instance) return instance;
		instance = new FlowWindowManagerX11();

		instance->display = XOpenDisplay(nullptr);
		if (!instance->display)
		{
			logger::error("Sorry, we failed to open a X display");
		}
		if (!setlocale(LC_CTYPE, "") || !XSupportsLocale()) {
			logger::warn("No locale support");
		}
		instance->root_window = DefaultRootWindow(instance->display);
		instance->screen = DefaultScreen(instance->display);
		instance->screen_width = DisplayWidth(instance->display, instance->screen);
		instance->screen_height = DisplayHeight(instance->display, instance->screen);

		XSetErrorHandler([](Display*, XErrorEvent*) -> int
		{
		  logger::error("OH NO, another wm is currently open");
		  std::exit(0);
		});// So We Can Output Custom Message

		XSelectInput(instance->display, instance->root_window, SubstructureRedirectMask);
		XSync(instance->display, false);
		XSetErrorHandler(nullptr);
		XSync(instance->display, false);

		instance->SetConfig(config);

		instance->drw = DrawableWindow::Create(
			instance->display,
			instance->screen,
			instance->root_window,
			instance->screen_width,
			instance->screen_height
		);

		if (!DrawableWindow::CreateFontSet(instance->drw, config->fonts))
		{
			logger::error("Can't load fonts");
			std::exit(-1);
		}


		instance->screen_manager = new ScreenManager();
		instance->screen_manager->UpdateGeom();
		instance->client_manager = new ClientManager();

		XSetWindowAttributes wa;
		Atom utf8string = XInternAtom(instance->display, "UFT8_STRING", false);
		instance->wm_atom[WMProtocols] = XInternAtom(instance->display, "WM_PROTOCOLS", False);
		instance->wm_atom[WMDelete] = XInternAtom(instance->display, "WM_DELETE_WINDOW", False);
		instance->wm_atom[WMState] = XInternAtom(instance->display, "WM_STATE", False);
		instance->wm_atom[WMTakeFocus] = XInternAtom(instance->display, "WM_TAKE_FOCUS", False);
		instance->net_atom[NetActiveWindow] = XInternAtom(instance->display, "_NET_ACTIVE_WINDOW", False);
		instance->net_atom[NetSupported] = XInternAtom(instance->display, "_NET_SUPPORTED", False);
		instance->net_atom[NetWMName] = XInternAtom(instance->display, "_NET_WM_NAME", False);
		instance->net_atom[NetWMState] = XInternAtom(instance->display, "_NET_WM_STATE", False);
		instance->net_atom[NetWMCheck] = XInternAtom(instance->display, "_NET_SUPPORTING_WM_CHECK", False);
		instance->net_atom[NetWMFullscreen] = XInternAtom(instance->display, "_NET_WM_STATE_FULLSCREEN", False);
		instance->net_atom[NetWMWindowType] = XInternAtom(instance->display, "_NET_WM_WINDOW_TYPE", False);
		instance->net_atom[NetWMWindowTypeDialog] = XInternAtom(instance->display, "_NET_WM_WINDOW_TYPE_DIALOG", False);
		instance->net_atom[NetClientList] = XInternAtom(instance->display, "_NET_CLIENT_LIST", False);

		instance->cursor[CurNormal] = CursorUtils::CreateCursor(instance->drw, XC_left_ptr);
		instance->cursor[CurResize] = CursorUtils::CreateCursor(instance->drw, XC_sizing);
		instance->cursor[CurMove] = CursorUtils::CreateCursor(instance->drw, XC_fleur);

		//TODO FIX THIS STARTING HERE
		static const char col_gray1[] = "#222222";
		static const char col_gray2[] = "#444444";
		static const char col_gray3[] = "#bbbbbb";
		static const char col_gray4[] = "#eeeeee";
		static const char col_cyan[] = "#005577";
		static const char* colors[][3] = {
			/*               fg         bg         border   */
			[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
			[SchemeSel]  = { col_gray4, col_cyan, col_cyan },
		};

		instance->color_scheme =
			static_cast<XftColor**>(malloc(sizeof(XftColor*) * sizeof(colors) / sizeof(colors[0])));
		for (unsigned int i = 0; i < sizeof(colors) / sizeof(colors[0]); i++)
		{
			instance->color_scheme[i] = ColorScheme::ScmCreate(instance->drw, colors[i], 3);
		}
		//TODO ENDING HERE

		instance->UpdateStatus();
		instance->wm_check_window = XCreateSimpleWindow(instance->display, instance->root_window, 0, 0, 1, 1, 0, 0, 0);

		XChangeProperty(instance->display,
			instance->wm_check_window,
			instance->net_atom[NetWMCheck],
			XA_WINDOW,
			32,
			PropModeReplace,
			(unsigned char*)&instance->wm_check_window,
			1
		);

		XChangeProperty(instance->display,
			instance->wm_check_window,
			instance->net_atom[NetWMName],
			utf8string,
			8,
			PropModeReplace,
			(unsigned char*)"flow-wm",
			3
		);

		XChangeProperty(instance->display,
			instance->root_window,
			instance->net_atom[NetWMCheck],
			XA_WINDOW,
			32,
			PropModeReplace,
			(unsigned char*)&instance->wm_check_window,
			1
		);

		XChangeProperty(instance->display,
			instance->root_window,
			instance->net_atom[NetSupported],
			XA_ATOM,
			32,
			PropModeReplace,
			(unsigned char*)instance->net_atom,
			NetLast
		);

		XDeleteProperty(instance->display, instance->root_window, instance->net_atom[NetClientList]);

		wa.cursor = instance->cursor[CurNormal]->cursor;
		wa.event_mask = SubstructureRedirectMask | SubstructureNotifyMask
			| ButtonPressMask | PointerMotionMask | EnterWindowMask
			| LeaveWindowMask | StructureNotifyMask | PropertyChangeMask;

		XChangeWindowAttributes(
			instance->display,
			instance->root_window,
			CWEventMask | CWCursor,
			&wa
		);

		XSelectInput(instance->display, instance->root_window, wa.event_mask);

		instance->keyboard_manager = new KeyboardManager(instance->config->key_bindings, instance->config->client_key_bindings, instance->config->mod_key);
		instance->keyboard_manager->Start(instance->display,instance-> root_window);


		instance->client_manager->FocusNull();
		instance->Scan();


		return instance;
	}

	FlowWindowManagerX11* FlowWindowManagerX11::Get()
	{
		return instance;
	}

	void FlowWindowManagerX11::SetConfig(Config* c)
	{
		delete this->config;
		this->config = c;
	}

	Display* FlowWindowManagerX11::GetDisplay()
	{
		return display;
	}

	Window FlowWindowManagerX11::GetRootWindow()
	{
		return root_window;
	}

	Atom* FlowWindowManagerX11::GetNetAtom()
	{
		return net_atom;
	}

	XftColor** FlowWindowManagerX11::GetColorScheme()
	{
		return color_scheme;
	}
	KeyboardManager* FlowWindowManagerX11::GetKeyboardManager()
	{
		return keyboard_manager;
	}

	Atom* FlowWindowManagerX11::GetWmAtom()
	{
		return wm_atom;
	}

	void FlowWindowManagerX11::Scan()
	{

		unsigned int i, num;
		Window d1, d2, * wins = NULL;
		XWindowAttributes wa;

		if (XQueryTree(display, root_window, &d1, &d2, &wins, &num))
		{
			for (i = 0; i < num; i++)
			{
				if (!XGetWindowAttributes(display, wins[i], &wa)
					|| wa.override_redirect || XGetTransientForHint(display, wins[i], &d1))
					continue;
				if (wa.map_state == IsViewable || ClientManager::GetState(wins[i]) == IconicState)
					client_manager->Manage(wins[i], &wa);
			}
			for (i = 0; i < num; i++)
			{ /* now the transients */
				if (!XGetWindowAttributes(display, wins[i], &wa))
					continue;
				if (XGetTransientForHint(display, wins[i], &d1)
					&& (wa.map_state == IsViewable || ClientManager::GetState(wins[i]) == IconicState))
					client_manager->Manage(wins[i], &wa);
			}
			if (wins)
				XFree(wins);
		}

	}

	ClientManager* FlowWindowManagerX11::GetClientManager()
	{
		return client_manager;
	}

	void FlowWindowManagerX11::UpdateStatus()
	{
		static char s_text[256] = "";
		if (!GetTextProp(root_window, XA_WM_NAME, s_text, sizeof(s_text)))
		{
			std::string version = std::string("flow-wm") + std::string(FLOW_WM_VERSION);
			strcpy(s_text, version.c_str());
		}
	}

	int FlowWindowManagerX11::GetTextProp(Window w, Atom atom, char* text, unsigned int size)
	{
		char** list = nullptr;
		int n;
		XTextProperty name;

		if (!text || size == 0)
			return 0;
		text[0] = '\0';
		if (!XGetTextProperty(display, w, &name, atom) || !name.nitems)
			return 0;
		if (name.encoding == XA_STRING)
			strncpy(text, (char*)name.value, size - 1);
		else
		{
			if (XmbTextPropertyToTextList(display, &name, &list, &n) >= Success && n > 0 && *list)
			{
				strncpy(text, *list, size - 1);
				XFreeStringList(list);
			}
		}
		text[size - 1] = '\0';
		XFree(name.value);
		return 1;
	}



}