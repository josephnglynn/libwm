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

	FlowWindowManagerX11* FlowWindowManagerX11::Init(Config* config)
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

		instance->SetConfig(config);

		int screen = DefaultScreen(instance->display);
		int sw = DisplayWidth(instance->display, screen);
		int sh = DisplayHeight(instance->display, screen);

		instance->drw = DrawableWindow::Create(instance->display, screen, instance->root_window, sw, sh);
		if (!DrawableWindow::CreateFontSet(instance->drw, instance, ))


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


	 DrawableWindow* DrawableWindow::Create(Display* display, int screen, Window root, unsigned int w, unsigned int h)
	{
		auto* drw = new DrawableWindow();

		drw->dpy = display;
		drw->screen = screen;
		drw->root = root;
		drw->w = w;
		drw->h = h;
		drw->drawable = XCreatePixmap(display, root, w, h, DefaultDepth(display, screen));
		drw->gc = XCreateGC(display, root, 0, nullptr);
		XSetLineAttributes(display, drw->gc, 1, LineSolid, CapButt, JoinMiter);

		return drw;
	}

	Fnt* DrawableWindow::CreateFontSet(DrawableWindow* drw, const char** fonts, size_t font_count)
	{
		Fnt *cur, *ret = nullptr;
		size_t i;

		if (!drw || !fonts)
			return nullptr;

		for (i = 1; i <= font_count; i++) {
			if ((cur = Fnt::XCreateFont(drw, fonts[font_count - i], nullptr))) {
				cur->next = ret;
				ret = cur;
			}
		}
		return (drw->fonts = ret);
	}


	Fnt* Fnt::XCreateFont(DrawableWindow* drw, const char* fontname, FcPattern* fontpattern)
	{
		{
			Fnt *font;
			XftFont *xfont = nullptr;
			FcPattern *pattern = nullptr;

			if (fontname) {
				if (!(xfont = XftFontOpenName(drw->dpy, drw->screen, fontname))) {
					logger::error("error, cannot load font from name: ", fontname);
					return nullptr;
				}
				if (!(pattern = FcNameParse((FcChar8 *) fontname))) {
					logger::error("error, cannot parse font name to pattern: ", fontname);
					XftFontClose(drw->dpy, xfont);
					return nullptr;
				}
			} else if (fontpattern) {
				if (!(xfont = XftFontOpenPattern(drw->dpy, fontpattern))) {
					logger::error( "error, cannot load font from pattern.");
					return nullptr;
				}
			} else {
				logger::error("No font specified");
				std::exit(-1);
			}

			FcBool is_col;
			if(FcPatternGetBool(xfont->pattern, FC_COLOR, 0, &is_col) == FcResultMatch && is_col) {
				XftFontClose(drw->dpy, xfont);
				return nullptr;
			}

			font = static_cast<Fnt*>(malloc(sizeof(Fnt)));
			font->xfont = xfont;
			font->pattern = pattern;
			font->h = xfont->ascent + xfont->descent;
			font->dpy = drw->dpy;

			return font;
		}
	}
}