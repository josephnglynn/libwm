#include <X11/Xlib.h>
#include <string>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <unistd.h>
#include "../../public/xlib/color_scheme/color_scheme.hpp"
#include "../../public/xlib/enums/enums.hpp"
#include "../../public/flow_wm_xlib.hpp"
#include "../../../external/logger/src/public/logger.hpp"
#include <csignal>
#include <sys/wait.h>

#ifndef FLOW_WM_VERSION
#define FLOW_WM_VERSION "unofficial-build"
#endif

namespace flow::X11
{
	FlowWindowManagerX11* FlowWindowManagerX11::instance;

	void FlowWindowManagerX11::HandleEvent(XEvent& event)
	{
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
			logger::info("WHATEVER EVENT ", std::to_string(event.type), " IS, WE MUST HAVE IGNORED IT");
			break;
		}
	}

	void HandleSignal(int signal)
	{
		auto fwm = FlowWindowManagerX11::Get();
		auto display = fwm->GetDisplay();
		auto cm = fwm->GetClientManager();
		auto base = fwm->GetBase();

		logger::success("Signal handler called... unwinding");
		Client* c;
		for (c = cm->GetFirst(); c; c = cm->GetFirst())
		{
			logger::info("Unmanaging client:", c->name);
			try
			{
				cm->UnManage(c, 0);
			}
			catch (std::exception& e)
			{
				logger::warn("An error occurred unmanaging client:", c->name);
			}
		}

		logger::info("Killing shell");
		if (base)
		{
			c = new Client(base);
			cm->KillClient(c);
			delete c;
		}

		XCloseDisplay(display);
		delete fwm;
		logger::error("EXITING");
		std::exit(signal);
	}

	void FlowWindowManagerX11::Start()
	{

		std::signal(SIGABRT, HandleSignal);
		std::signal(SIGFPE, HandleSignal);
		std::signal(SIGILL, HandleSignal);
		std::signal(SIGINT, HandleSignal);
		std::signal(SIGSEGV, HandleSignal);
		std::signal(SIGTERM, HandleSignal);

		while (!quit)
		{
			XEvent event;
			XNextEvent(display, &event);
			Client* c;
			if (event.type == MotionNotify) continue;
			if (event.xany.window == base)
			{
				shell->HandleEventBase(&event, 0, 0, screen_width, screen_height);
			}
			if ((c = client_manager->GetClientFromFrame(event.xany.window)))
			{
				shell->HandleEventFrame(&event, c->position.x, c->position.y, c->position.width, c->position.height);
				if (event.xbutton.subwindow == c->window)
				{
					event.xbutton.window = c->window;
					OnButtonPress(event);
				}
				continue;
			}
			HandleEvent(event);
		}
	}

	int FlowX11ErrorHandler(Display*, XErrorEvent* event)
	{
		logger::warn(
			"An X11 Error Occurred! Don't worry though, it will not stop execution\nError code: ",
			std::to_string(event->error_code),
			"\nType: " + std::to_string(event->type)
		);
		return 0;
	}

	FlowWindowManagerX11::~FlowWindowManagerX11()
	{
		if (!detached) XCloseDisplay(display);
		delete screen_manager;
		delete keyboard_manager;
		delete client_manager;
		delete drw;
		delete shell;
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

		if (!setlocale(LC_CTYPE, "") || !XSupportsLocale())
		{
			logger::warn("No locale support");
		}

		instance->root_window = DefaultRootWindow(instance->display);
		instance->screen = DefaultScreen(instance->display);
		instance->screen_width = DisplayWidth(instance->display, instance->screen);
		instance->screen_height = DisplayHeight(instance->display, instance->screen);

#ifdef DEBUG
		XSynchronize(instance->display, true);
#endif

		XSetErrorHandler([](Display*, XErrorEvent*) -> int
		{
		  logger::error("OH NO, another wm is currently open");
		  std::exit(0);
		});// So We Can Output Custom Message

		XSelectInput(instance->display, instance->root_window, SubstructureRedirectMask);
		XSync(instance->display, false);
		XSetErrorHandler(FlowX11ErrorHandler);
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

		//TODO FIX SHELL
		instance->shell = new Shell(config->shell_location);
		instance->shell->OnLoad([](XEvent* event)
		{
		  instance->HandleEvent(*event);
		});

		instance->screen_manager = new ScreenManager();
		instance->client_manager = new ClientManager();
		instance->screen_manager->UpdateGeom();

		XSetWindowAttributes wa;
		instance->utf8string = XInternAtom(instance->display, "UFT8_STRING", false);
		instance->wm_atom[WMProtocols] = XInternAtom(instance->display, "WM_PROTOCOLS", false);
		instance->wm_atom[WMDelete] = XInternAtom(instance->display, "WM_DELETE_WINDOW", false);
		instance->wm_atom[WMState] = XInternAtom(instance->display, "WM_STATE", false);
		instance->wm_atom[WMTakeFocus] = XInternAtom(instance->display, "WM_TAKE_FOCUS", false);
		instance->wm_atom[WMName] = XInternAtom(instance->display, "WM_NAME", false);
		instance->net_atom[NetActiveWindow] = XInternAtom(instance->display, "_NET_ACTIVE_WINDOW", false);
		instance->net_atom[NetSupported] = XInternAtom(instance->display, "_NET_SUPPORTED", false);
		instance->net_atom[NetWMName] = XInternAtom(instance->display, "_NET_WM_NAME", false);
		instance->net_atom[NetWMState] = XInternAtom(instance->display, "_NET_WM_STATE", false);
		instance->net_atom[NetWMCheck] = XInternAtom(instance->display, "_NET_SUPPORTING_WM_CHECK", false);
		instance->net_atom[NetWMFullscreen] = XInternAtom(instance->display, "_NET_WM_STATE_FULLSCREEN", false);
		instance->net_atom[NetWMWindowType] = XInternAtom(instance->display, "_NET_WM_WINDOW_TYPE", false);
		instance->net_atom[NetWMWindowTypeDialog] = XInternAtom(instance->display, "_NET_WM_WINDOW_TYPE_DIALOG", false);
		instance->net_atom[NetClientList] = XInternAtom(instance->display, "_NET_CLIENT_LIST", false);
		instance->net_atom[NetWMTypeDesk] = XInternAtom(instance->display, "_NET_WM_WINDOW_TYPE_DESKTOP", false);
		instance->net_atom[NetWMTypeDock] = XInternAtom(instance->display, "_NET_WM_WINDOW_TYPE_DESKTOP", false);
		instance->net_atom[NetWMStateFs] = XInternAtom(instance->display, "_NET_WM_STATE_FULLSCREEN", false);
		instance->net_atom[NetWMStateModel] = XInternAtom(instance->display, "_NET_WM_STATE_MODAL", false);
		instance->net_atom[NetMWMHints] = XInternAtom(instance->display, "_MOTIF_WM_HINTS", false);
		instance->net_atom[NetWindowOpacity] = XInternAtom(instance->display, "_NET_WM_WINDOW_OPACITY", false);

		instance->cursor[CurNormal] = CursorUtils::CreateCursor(instance->drw, XC_left_ptr);
		instance->cursor[CurResizeTopLeft] = CursorUtils::CreateCursor(instance->drw, XC_top_left_corner);
		instance->cursor[CurResizeTopMiddle] = CursorUtils::CreateCursor(instance->drw, XC_top_side);
		instance->cursor[CurResizeTopRight] = CursorUtils::CreateCursor(instance->drw, XC_top_right_corner);
		instance->cursor[CurResizeCenterLeft] = CursorUtils::CreateCursor(instance->drw, XC_left_side);
		instance->cursor[CurResizeCenterMiddle] = CursorUtils::CreateCursor(instance->drw, XC_fleur);
		instance->cursor[CurResizeCenterRight] = CursorUtils::CreateCursor(instance->drw, XC_right_side);
		instance->cursor[CurResizeBottomRight] = CursorUtils::CreateCursor(instance->drw, XC_bottom_right_corner);
		instance->cursor[CurResizeBottomMiddle] = CursorUtils::CreateCursor(instance->drw, XC_bottom_side);
		instance->cursor[CurResizeBottomLeft] = CursorUtils::CreateCursor(instance->drw, XC_bottom_left_corner);
		instance->cursor[CurMove] = CursorUtils::CreateCursor(instance->drw, XC_fleur);

		instance->UpdateStatus();
		instance->wm_check_window = XCreateSimpleWindow(instance->display, instance->root_window, 0, 0, 1, 1, 0, 0, 0);

		XChangeProperty(
			instance->display,
			instance->wm_check_window,
			instance->net_atom[NetWMCheck],
			XA_WINDOW,
			32,
			PropModeReplace,
			(unsigned char*)&instance->wm_check_window,
			1
		);

		XChangeProperty(
			instance->display,
			instance->wm_check_window,
			instance->net_atom[NetWMName],
			instance->utf8string,
			8,
			PropModeReplace,
			(unsigned char*)"flow-wm",
			3
		);

		XChangeProperty(
			instance->display,
			instance->root_window,
			instance->net_atom[NetWMCheck],
			XA_WINDOW,
			32,
			PropModeReplace,
			(unsigned char*)&instance->wm_check_window,
			1
		);

		XChangeProperty(
			instance->display,
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

		instance->keyboard_manager = new KeyboardManager(
			instance->config->key_bindings,
			instance->config->client_key_bindings,
			instance->config->mod_key
		);

#ifdef DEBUG
		XSetErrorHandler(nullptr);
#endif
		instance->base = instance->shell->CreateBackWindow(
			0,
			0,
			instance->screen_width,
			instance->screen_height,
			instance->display,
			instance->root_window
		);


		instance->keyboard_manager->Start(instance->display, instance->root_window);
		instance->client_manager->Focus(nullptr);
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

	void FlowWindowManagerX11::Scanner(Window window)
	{
		unsigned int num;
		Window d1, d2, * wins = nullptr;
		XWindowAttributes wa;

		if (XQueryTree(display, window, &d1, &d2, &wins, &num))
		{
			for (unsigned int i = 0; i < num; i++)
			{
				if (wins[i] == base) continue;
				if (!XGetWindowAttributes(display, wins[i], &wa) || wa.override_redirect || wa.map_state != IsViewable
					|| XGetTransientForHint(display, wins[i], &d1))
				{
					continue;
				}

				if (wa.map_state == IsViewable || ClientManager::GetState(wins[i]) == IconicState)
				{
					suicide_list.push_back(wins[i]);
					client_manager->Manage(wins[i], &wa);
				}

			}

			for (unsigned int i = 0; i < num; i++)
			{
				if (wins[i] == base) continue;
				if (!XGetWindowAttributes(display, wins[i], &wa))
				{
					continue;
				}

				if (XGetTransientForHint(display, wins[i], &d1)
					&& (wa.map_state == IsViewable || ClientManager::GetState(wins[i]) == IconicState))
				{
					suicide_list.push_back(wins[i]);
					client_manager->Manage(wins[i], &wa);
				}

			}

			if (wins)
			{
				XFree(wins);
			}
		}
	}

	void FlowWindowManagerX11::Scan()
	{
		Scanner(root_window);
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

	void FlowWindowManagerX11::Detach()
	{
		if (display)
		{
			close(ConnectionNumber(display));
		}
		detached = true;
	}

}
