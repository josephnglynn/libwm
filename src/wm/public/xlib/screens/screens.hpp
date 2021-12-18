//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_SCREENS_HPP
#define FLOW_WM_SCREENS_HPP
#include <X11/Xlib.h>
#include <X11/extensions/Xinerama.h>
#include <vector>
#include "../../general/shapes.hpp"


namespace flow::X11 {
	class  ClientManager;
	struct Client;
}

namespace flow
{


	struct Monitor
	{
		int num;
		int by;
		int mx, my, mw, mh;
		int wx, wy, ww, wh;
		X11::ClientManager* clients;
		Monitor *next;
		int screen;
	};

	class ScreenManager
	{
	public:
		ScreenManager() = default;
		int UpdateGeom();
		int IsUniqueGeom(XineramaScreenInfo *unique, size_t n, XineramaScreenInfo *info);
		void CleanUpMonitor(Monitor* monitor);

		void Focus(X11::Client* client);
		void UnFocus(X11::Client* client, int set_focus);
		void Manage(Window window, XWindowAttributes* wa);
		void UnManage(X11::Client* client, int destroyed);
		void Resize(X11::Client* client, int x, int y, int w, int h, int interact);

		Monitor* GetSelectedMonitor();
		Monitor* GetMons();
		void SetSelectedMonitor(Monitor* monitor);
		void ReStack(Monitor* m);
		void Arrange(Monitor* m);

		Monitor* WindowToMonitor(Window w);
		Monitor* CreateMonitor();
		Monitor* RectToMonitor(shapes::Rectangle rectangle);
		X11::Client* WindowToClient(Window w);
	private:
		Monitor* mons = nullptr;
		Monitor* selected_monitor = nullptr;
	};

}

#endif //FLOW_WM_SCREENS_HPP
