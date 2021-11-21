//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_SCREENS_HPP
#define FLOW_WM_SCREENS_HPP
#include <X11/Xlib.h>
#include "../../general/shapes.hpp"
#include <X11/extensions/Xrandr.h>
#include <vector>

namespace flow
{

	struct Monitor : _XRRMonitorInfo
	{
		Monitor() : _XRRMonitorInfo(){}
		explicit Monitor(_XRRMonitorInfo info) : _XRRMonitorInfo(info){}

		shapes::Rectangle toRectangle();
	};

	class ScreenManager
	{
	public:
		static void Init(Display* display, Window rootWindow);
		static ScreenManager* Get();
		Monitor* GetClientMonitor(shapes::Rectangle windowPos);
	private:
		std::vector<Monitor*> monitors;
		static ScreenManager* instance;
	};

}

#endif //FLOW_WM_SCREENS_HPP
