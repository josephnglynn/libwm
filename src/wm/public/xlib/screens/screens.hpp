//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_SCREENS_HPP
#define FLOW_WM_SCREENS_HPP
#include <X11/Xlib.h>
#include <X11/extensions/Xinerama.h>
#include <vector>
#include "../../general/shapes.hpp"
#include <glob.h>

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
		Monitor *next;
		int screen;
	};

	class ScreenManager
	{
	public:
		ScreenManager() = default;

		int UpdateGeom();
		int IsUniqueGeom(XineramaScreenInfo *unique, size_t n, XineramaScreenInfo *info);



		Monitor* GetSelectedMonitor();
		Monitor* GetMons();
		Monitor* WindowToMonitor(Window w);
		Monitor* CreateMonitor();
		Monitor* RectToMonitor(shapes::Rectangle rectangle);
		Monitor* GetCursorMonitor();
		Monitor* GetMonitor(int x, int y);

		void SetSelectedMonitor(Monitor* monitor);
		void ReStack();
		void Arrange(Monitor* m);
		void CleanUpMonitor(Monitor* monitor);

		Monitor* selected_monitor = nullptr;
	private:
		Monitor* mons = nullptr;

	};

}

#endif //FLOW_WM_SCREENS_HPP
