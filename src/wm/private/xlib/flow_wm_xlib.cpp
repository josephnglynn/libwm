#include "flow_wm_xlib.hpp"
#include "public/xlib/handlers/handlers.hpp"
#include "../../../logger/public/logger.hpp"
#include <X11/Xlib.h>
#include <string>


namespace flow::X11
{

	FlowWindowManagerX11* FlowWindowManagerX11::instance;

	void FlowWindowManagerX11::Start()
	{

		display = XOpenDisplay(nullptr);
		if (!display)
		{
			logger::error("Sorry, we failed to open a X display");
		}

		rootWindow = DefaultRootWindow(display);

		XSetErrorHandler([](Display* d, XErrorEvent* event) -> int
		{
		  logger::error("OH NO, another wm is currently open");
		  std::exit(0);
		});// So We Can Output Custom Message

		XSelectInput(display, rootWindow, SubstructureRedirectMask | SubstructureNotifyMask | ButtonPressMask);

		XSync(display, false);
		XSetErrorHandler(FlowX11ErrorHandler);

		screenManager = new ScreenManager(display, rootWindow);
		clientManager = new ClientManager();

		while (!quit)
		{

			XEvent event;
			XNextEvent(display, &event);

			logger::info("EVENT ", std::to_string(event.type), " OCCURRED");


			switch (event.type)
			{
			case KeyPress:
				handlers::onKeyPress(event);
				break;
			case KeyRelease:
				handlers::onKeyRelease(event);
				break;
			case ButtonPress:
				handlers::onButtonPress(event);
				break;
			case ButtonRelease:
				handlers::onButtonRelease(event);
				break;
			case MotionNotify:
				handlers::onMotionNotify(event);
				break;
			case EnterNotify:
				handlers::onEnterNotify(event);
				break;
			case LeaveNotify:
				handlers::onLeaveNotify(event);
				break;
			case FocusIn:
				handlers::onFocusIn(event);
				break;
			case FocusOut:
				handlers::onFocusOut(event);
				break;
			case KeymapNotify:
				handlers::onKeymapNotify(event);
				break;
			case Expose:
				handlers::onExpose(event);
				break;
			case GraphicsExpose:
				handlers::onGraphicsExpose(event);
				break;
			case NoExpose:
				handlers::onNoExpose(event);
				break;
			case VisibilityNotify:
				handlers::onVisibilityNotify(event);
				break;
			case CreateNotify:
				handlers::onCreateNotify(event);
				break;
			case DestroyNotify:
				handlers::onDestroyNotify(event);
				break;
			case UnmapNotify:
				handlers::onUnmapNotify(event);
				break;
			case MapNotify:
				handlers::onMapNotify(event);
				break;
			case MapRequest:
				handlers::onMapRequest(event);
				break;
			case ReparentNotify:
				handlers::onReparentNotify(event);
				break;
			case ConfigureNotify:
				handlers::onConfigureNotify(event);
				break;
			case ConfigureRequest:
				handlers::onConfigureRequest(event);
				break;
			case GravityNotify:
				handlers::onGravityNotify(event);
				break;
			case ResizeRequest:
				handlers::onResizeRequest(event);
				break;
			case CirculateNotify:
				handlers::onCirculateNotify(event);
				break;
			case CirculateRequest:
				handlers::onCirculateNotify(event);
				break;
			case PropertyNotify:
				handlers::onPropertyNotify(event);
				break;
			case SelectionClear:
				handlers::onSelectionClear(event);
				break;
			case SelectionRequest:
				handlers::onSelectionRequest(event);
				break;
			case SelectionNotify:
				handlers::onSelectionNotify(event);
				break;
			case ColormapNotify:
				handlers::onColormapNotify(event);
				break;
			case ClientMessage:
				handlers::onClientMessage(event);
				break;
			case MappingNotify:
				handlers::onMappingNotify(event);
				break;
			case GenericEvent:
				handlers::onGenericEvent(event);
				break;
			case LASTEvent:
				handlers::onLastEvent(event);
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
		instance = new FlowWindowManagerX11();
		return instance;
	}

	FlowWindowManagerX11* FlowWindowManagerX11::Get()
	{
		if (!instance) return Init();
		return instance;
	}

	Display* FlowWindowManagerX11::GetDisplay()
	{
		return display;
	}

}