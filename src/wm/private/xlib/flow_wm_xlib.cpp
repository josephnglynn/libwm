#include "flow_wm_xlib.hpp"
#include "../../../logger/public/logger.hpp"
#include <X11/Xlib.h>
#include <string>

namespace flow::X11
{

	FlowWindowManagerX11* FlowWindowManagerX11::instance;

	void FlowWindowManagerX11::Start()
	{

		screen_manager = new ScreenManager(display, root_window);
		client_manager = new ClientManager();


		while (!quit)
		{

			XEvent event;
			XNextEvent(display, &event);

			logger::info("EVENT ", std::to_string(event.type), " OCCURRED");

			switch (event.type)
			{
			case KeyPress:
				onKeyPress(event);
				break;
			case KeyRelease:
				onKeyRelease(event);
				break;
			case ButtonPress:
				onButtonPress(event);
				break;
			case ButtonRelease:
				onButtonRelease(event);
				break;
			case MotionNotify:
				onMotionNotify(event);
				break;
			case EnterNotify:
				onEnterNotify(event);
				break;
			case LeaveNotify:
				onLeaveNotify(event);
				break;
			case FocusIn:
				onFocusIn(event);
				break;
			case FocusOut:
				onFocusOut(event);
				break;
			case KeymapNotify:
				onKeymapNotify(event);
				break;
			case Expose:
				onExpose(event);
				break;
			case GraphicsExpose:
				onGraphicsExpose(event);
				break;
			case NoExpose:
				onNoExpose(event);
				break;
			case VisibilityNotify:
				onVisibilityNotify(event);
				break;
			case CreateNotify:
				onCreateNotify(event);
				break;
			case DestroyNotify:
				onDestroyNotify(event);
				break;
			case UnmapNotify:
				onUnmapNotify(event);
				break;
			case MapNotify:
				onMapNotify(event);
				break;
			case MapRequest:
				onMapRequest(event);
				break;
			case ReparentNotify:
				onReparentNotify(event);
				break;
			case ConfigureNotify:
				onConfigureNotify(event);
				break;
			case ConfigureRequest:
				onConfigureRequest(event);
				break;
			case GravityNotify:
				onGravityNotify(event);
				break;
			case ResizeRequest:
				onResizeRequest(event);
				break;
			case CirculateNotify:
				onCirculateNotify(event);
				break;
			case CirculateRequest:
				onCirculateNotify(event);
				break;
			case PropertyNotify:
				onPropertyNotify(event);
				break;
			case SelectionClear:
				onSelectionClear(event);
				break;
			case SelectionRequest:
				onSelectionRequest(event);
				break;
			case SelectionNotify:
				onSelectionNotify(event);
				break;
			case ColormapNotify:
				onColormapNotify(event);
				break;
			case ClientMessage:
				onClientMessage(event);
				break;
			case MappingNotify:
				onMappingNotify(event);
				break;
			case GenericEvent:
				onGenericEvent(event);
				break;
			case LASTEvent:
				onLastEvent(event);
				break;
			default:
				logger::warn("WHATEVER EVENT ", std::to_string(event.type), " IS, WE MUST HAVE IGNORED IT");
				break;
			}
		}
	}

	int FlowX11ErrorHandler(Display* display, XErrorEvent* event)
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

	FlowWindowManagerX11* FlowWindowManagerX11::Init()
	{
		if (instance) return instance;
		instance = new FlowWindowManagerX11();


		instance->display = XOpenDisplay(nullptr);
		if (!instance->display)
		{
			logger::error("Sorry, we failed to open a X display");
		}

		instance->root_window = DefaultRootWindow(instance->display);

		XSetErrorHandler([](Display* d, XErrorEvent* event) -> int
		{
		  logger::error("OH NO, another wm is currently open");
		  std::exit(0);
		});// So We Can Output Custom Message

		XSelectInput(instance->display,
			instance->root_window,
			SubstructureRedirectMask
				| SubstructureNotifyMask
				| ButtonPressMask
				| KeyPressMask
					//| PointerMotionMask
				| EnterWindowMask
				| LeaveWindowMask
				| StructureNotifyMask
				| PropertyChangeMask
		);

		{
			auto cursor = XCreateFontCursor(instance->display, 1);
			XDefineCursor(instance->display, instance->root_window, cursor);
		}



		XSync(instance->display, false);
		XSetErrorHandler(FlowX11ErrorHandler);

		return instance;
	}

	FlowWindowManagerX11* FlowWindowManagerX11::Get()
	{
		if (!instance) return Init();
		return instance;
	}


	void FlowWindowManagerX11::SetConfig(Config* config)
	{
		delete keyboard_manager;
		keyboard_manager = new KeyboardManager(config->key_bindings, config->mod_key);
		keyboard_manager->Start(display, root_window);
	}

	Display* FlowWindowManagerX11::GetDisplay()
	{
		return display;
	}
}