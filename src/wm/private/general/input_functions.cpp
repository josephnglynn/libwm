//
// Created by joseph on 27/11/2021.
//

#include <cstdlib>
#include "../../public/general/input_functions.hpp"
#include "../../public/general/inline_functions.hpp"
#include "../../../logger/public/logger.hpp"
#include "../../public/general/masks.hpp"

using namespace flow::X11;

namespace flow::input_functions
{

	std::function<void(const std::string&)> InputFunctionToFunctionPointer(InputFunction i_f)
	{
		switch (i_f)
		{
		case Spawn:
			return spawn;
		case MoveMouse:
			return moveMouse;
		case ResizeMouse:
			return resizeMouse;
		case KillClient:
			return killClient;
		case FocusClient:
			return focusClient;
		case ReloadShell:
			return reloadShell;
		}
	}

	std::vector<std::string> split(std::string s)
	{

		std::vector<std::string> strings;
		while (!s.empty())
		{

			size_t pos = s.find(' ');
			if (pos == 0)
			{
				s.erase(s.begin(), s.begin() + 1);
				continue;
			}

			if (pos == std::string::npos)
			{
				strings.push_back(s);
				break;
			}

			strings.push_back(s.substr(0, pos));
			s.erase(s.begin(), s.begin() + static_cast<long>(pos));

		}

		return strings;
	}

	void focusClient(const std::string&)
	{
		auto fwm = X11::FlowWindowManagerX11::Get();
		auto sm = fwm->GetScreenManager();
		logger::info("FOCUS CLIENT");
		sm->Focus(sm->GetSelectedMonitor()->clients->selected);
	}

	void spawn(const std::string& arg)
	{
		auto fwm = X11::FlowWindowManagerX11::Get();
		if (fork() == 0)
		{
			fwm->Detach();
			setsid();
			std::vector<std::string> split_string = split(arg);
			char** args = new char* [split_string.size() + 1];
			for (size_t i = 0; i < split_string.size(); ++i)
			{
				args[i] = strdup(split_string[i].c_str());
			}
			args[split_string.size()] = nullptr;
			execvp(args[0], args);
			_exit(0);
		}

	}

	int GetRootPointer(int* x, int* y)
	{
		auto fwm = X11::FlowWindowManagerX11::Get();
		int di;
		unsigned int dui;
		Window dummy;

		return XQueryPointer(fwm->GetDisplay(), fwm->GetRootWindow(), &dummy, &dummy, x, y, &di, &di, &dui);
	}

	inline int int_abs(int x)
	{
		return abs(x);
	}

	void moveMouse(const std::string&)
	{
		auto fwm = X11::FlowWindowManagerX11::Get();
		ScreenManager* sm = fwm->GetScreenManager();
		int x, y, ocx, ocy, nx, ny;
		Client* c;
		Monitor* m;
		XEvent event;
		Time last_time;
		Monitor* sel_mon = sm->GetSelectedMonitor();

		if (!(c = sel_mon->clients->selected)) return;
		if (c->full_screen) return;

		sm->ReStack(sel_mon);
		ocx = c->position.x;
		ocy = c->position.y;

		if (XGrabPointer(fwm->GetDisplay(), fwm->GetRootWindow(), False, MOUSE_MASK, GrabModeAsync, GrabModeAsync,
			None, fwm->GetCursor()[CurMove]->cursor, CurrentTime) != GrabSuccess)
		{
			return;
		}

		if (!GetRootPointer(&x, &y)) return;

		do
		{
			XMaskEvent(fwm->GetDisplay(), MOUSE_MASK | ExposureMask | SubstructureRedirectMask, &event);
			switch (event.type)
			{
			case ConfigureRequest:
			case Expose:
			case MapRequest:
				fwm->HandleEvent(event);
				break;
			case MotionNotify:
				if ((event.xmotion.time - last_time) <= (1000 / 60))
					continue;
				last_time = event.xmotion.time;

				nx = ocx + (event.xmotion.x - x);
				ny = ocy + (event.xmotion.y - y);

				unsigned int snap = fwm->GetConfig()->snap;

				Monitor* selected_monitor = fwm->GetScreenManager()->GetSelectedMonitor();
				if (abs(selected_monitor->wx - nx) < static_cast<int>(snap))
					nx = selected_monitor->wx;
				else if (
					int_abs((selected_monitor->wx + selected_monitor->ww) - (nx + static_cast<int>(c->position.width)))
						< static_cast<int>(snap))
					nx = selected_monitor->wx + selected_monitor->ww - static_cast<int>(c->position.width);
				if (abs(selected_monitor->wy - ny) < static_cast<int>(snap))
					ny = selected_monitor->wy;
				else if (
					int_abs((selected_monitor->wy + selected_monitor->wh) - (ny + static_cast<int>(c->position.height)))
						< static_cast<int>(snap))
					ny = selected_monitor->wy + selected_monitor->wh - static_cast<int>(c->position.height);

				fwm->GetScreenManager()->Resize(
					c,
					nx,
					ny,
					static_cast<int>(c->position.width),
					static_cast<int>(c->position.height),
					1
				);
				break;
			}
		} while (event.type != ButtonRelease);

		XUngrabPointer(fwm->GetDisplay(), CurrentTime);
		if ((m = sm->RectToMonitor(c->position)) != sm->GetSelectedMonitor())
		{
			c->SendMonitor(m);
			sm->SetSelectedMonitor(m);
			sm->Focus(nullptr);
		}
	}

	enum MousePlacement
	{
		TOP_OR_LEFT,
		CENTER_MIDDLE,
		BOTTOM_OR_RIGHT
	};

	struct PositionOfMouse
	{
		MousePlacement vertical;
		MousePlacement horizontal;
		Cur* cursor;

		inline void WorkOut(int x, int y, int a_third_x, int a_third_y)
		{
			if (x < a_third_x)
			{
				horizontal = TOP_OR_LEFT;
			}
			else if (x < a_third_x * 2)
			{
				horizontal = CENTER_MIDDLE;
			}
			else
			{
				horizontal = BOTTOM_OR_RIGHT;
			}

			if (y < a_third_y)
			{
				vertical = TOP_OR_LEFT;
			}
			else if (y < a_third_y * 2)
			{
				vertical = CENTER_MIDDLE;
			}
			else
			{
				vertical = BOTTOM_OR_RIGHT;
			}
		}

		PositionOfMouse(int x, int y, int w, int h)
		{
			int a_third_x = w / 3;
			int a_third_y = h / 3;

			WorkOut(x, y, a_third_x, a_third_y);
			while (vertical == CENTER_MIDDLE && horizontal == CENTER_MIDDLE)
			{
				x -= a_third_x;
				y -= a_third_y;
				a_third_x /= 3;
				a_third_y /= 3;
				WorkOut(x, y, a_third_x, a_third_y);
			}

			auto fwm = FlowWindowManagerX11::Get();
			cursor = fwm->GetCursor()[vertical + horizontal * 3];
		}
	};

	void resizeMouse(const std::string&)
	{
		auto fwm = FlowWindowManagerX11::Get();
		ScreenManager* sm = fwm->GetScreenManager();
		int ocx, ocy, nw, nh;
		int ocx2, ocy2, nx, ny;
		Client* c;
		Monitor* m;
		XEvent event;
		int hc, vc;
		int root;
		unsigned int mask;
		Window dummy;
		Time last_time = 0;
		Monitor* sel_mon = sm->GetSelectedMonitor();

		if (!(c = sel_mon->clients->selected))
			return;
		if (c->full_screen)
			return;
		sm->ReStack(sel_mon);

		ocx = c->position.x;
		ocy = c->position.y;
		ocx2 = c->position.x + static_cast<int>(c->position.width);
		ocy2 = c->position.y + static_cast<int>(c->position.height);

		if (!XQueryPointer(fwm->GetDisplay(), c->window, &dummy, &dummy, &root, &root, &nx, &ny, &mask)) return;

		int center_x = static_cast<int>(c->position.width) / 2;
		int center_y = static_cast<int>(c->position.height) / 2;

		hc = nx < center_x;
		vc = ny < center_y;

		PositionOfMouse positionOfMouse = PositionOfMouse(nx, ny, c->position.width, c->position.height);

		if (XGrabPointer(fwm->GetDisplay(), fwm->GetRootWindow(), False, MOUSE_MASK, GrabModeAsync, GrabModeAsync,
			None, positionOfMouse.cursor->cursor, CurrentTime) != GrabSuccess) // CHECK ENUM IF BAFFED
			return;

		int x, y;

		switch (positionOfMouse.horizontal)
		{
		case TOP_OR_LEFT:
			x = -c->border_width;
			break;
		case CENTER_MIDDLE:
			x = center_x;
			break;
		case BOTTOM_OR_RIGHT:
			x = c->position.width + c->border_width;
			break;
		}

		switch (positionOfMouse.vertical)
		{
		case TOP_OR_LEFT:
			y = -c->border_width;
			break;
		case CENTER_MIDDLE:
			y = center_y;
			break;
		case BOTTOM_OR_RIGHT:
			y = c->position.height + c->border_width;
			break;
		}

		XWarpPointer(
			fwm->GetDisplay(),
			None,
			c->window,
			0,
			0,
			0,
			0,
			x,
			y
		);

		do
		{
			XMaskEvent(fwm->GetDisplay(), MOUSE_MASK | ExposureMask | SubstructureRedirectMask, &event);
			switch (event.type)
			{
			case ConfigureRequest:
			case Expose:
			case MapRequest:
				fwm->HandleEvent(event);
				break;
			case MotionNotify:
				if ((event.xmotion.time - last_time) <= (1000 / 60))
					continue;
				last_time = event.xmotion.time;

				if (positionOfMouse.vertical == CENTER_MIDDLE)
				{
					nx = hc ? event.xmotion.x : c->position.x;
					nw = Max(hc ? (ocx2 - nx) : (event.xmotion.x - ocx - 2 * c->border_width + 1), 1);
					sm->Resize(c, nx, c->position.y, nw, c->position.height, 1);
				}
				else if (positionOfMouse.horizontal == CENTER_MIDDLE)
				{
					ny = vc ? event.xmotion.y : c->position.y;
					nh = Max(vc ? (ocy2 - ny) : (event.xmotion.y - ocy - 2 * c->border_width + 1), 1);
					sm->Resize(c, c->position.x, ny, c->position.width, nh, 1);
				}
				else
				{
					nx = hc ? event.xmotion.x : c->position.x;
					ny = vc ? event.xmotion.y : c->position.y;
					nw = Max(hc ? (ocx2 - nx) : (event.xmotion.x - ocx - 2 * c->border_width + 1), 1);
					nh = Max(vc ? (ocy2 - ny) : (event.xmotion.y - ocy - 2 * c->border_width + 1), 1);
					sm->Resize(c, nx, ny, nw, nh, 1);
				}

				break;
			}
		} while (event.type != ButtonRelease);

		XWarpPointer(
			fwm->GetDisplay(),
			None,
			c->window,
			0,
			0,
			0,
			0,
			x,
			y
		);

		XUngrabPointer(fwm->GetDisplay(), CurrentTime);
		while (XCheckMaskEvent(fwm->GetDisplay(), EnterWindowMask, &event));
		if ((m = sm->RectToMonitor(c->position)) != sm->GetSelectedMonitor())
		{
			c->SendMonitor(m);
			sm->SetSelectedMonitor(m);
			sm->Focus(nullptr);
		}
	}

	void killClient(const std::string&)
	{
		auto fwm = FlowWindowManagerX11::Get();
		Client* selected = fwm->GetScreenManager()->GetSelectedMonitor()->clients->selected;
		if (!selected) return;
		if (!selected->SendEvent(fwm->GetWmAtom()[WMDelete]))
		{
			XGrabServer(fwm->GetDisplay());
			XSetErrorHandler([](Display*, XErrorEvent*) -> int
			{ return 0; });
			XSetCloseDownMode(fwm->GetDisplay(), DestroyAll);
			XKillClient(fwm->GetDisplay(), selected->window);
			XSync(fwm->GetDisplay(), false);
			XSetErrorHandler(nullptr);
			XUngrabServer(fwm->GetDisplay());
		}
	}

	void reloadShell(const std::string&)
	{
		auto fwm = FlowWindowManagerX11::Get();
		*fwm->GetShell() = Shell(fwm->GetConfig()->shell_location);
#ifdef DEBUG
		auto config = flow::Config::FromFilePath("config.json");
#else
		auto config = flow::Config::GetDefault();
#endif
		*fwm->GetConfig() = *config;
		fwm->GetKeyboardManager()->Update(config->key_bindings, config->client_key_bindings, config->mod_key);
		delete config;

	}
}

