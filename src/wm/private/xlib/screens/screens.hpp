//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_SCREENS_HPP
#define FLOW_WM_SCREENS_HPP
#include <X11/Xlib.h>
#include "../../general/shapes.hpp"
#include <X11/extensions/Xrandr.h>

namespace flow
{

	struct Monitor : _XRRMonitorInfo
	{
		Monitor* next = nullptr;

		Monitor() : _XRRMonitorInfo() {}
		explicit Monitor(_XRRMonitorInfo info) : _XRRMonitorInfo(info) {}

		shapes::Rectangle toRectangle();
	};

	class ScreenManager
	{
	public:
		void AddMonitor(Monitor* monitor);

		static void Init(Display* display, Window rootWindow);

		static Monitor* GetMonitor(Display* display, Window window, int* num);
	private:
		Monitor* first;
		Monitor* last;
		static ScreenManager* instance;
	};

}

#endif //FLOW_WM_SCREENS_HPP
