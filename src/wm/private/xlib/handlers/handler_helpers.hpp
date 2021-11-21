//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_HANDLER_HELPERS_HPP
#define FLOW_WM_HANDLER_HELPERS_HPP
#include "../../general/shapes.hpp"
#include <X11/extensions/Xrandr.h>
using namespace flow::shapes;

namespace flow::handler_helpers
{


	Monitor* GetClientMonitor(X11::Client c) {

	}


	Monitor* ReorderExcessMonitors(Monitor* m, int num, Rectangle shape) {

		if (num > 1 && (m[0].width - (static_cast<int>(shape.width) / 2) > shape.x))
		{
			m[0] = m[1];
		}

		return m;
	}

	void CenterChild(Display* display, X11::Client* client)
	{
		Rectangle rectangle = Rectangle();
		int numOfMonitors;
		auto monitors = ScreenManager::GetMonitor(display, client->window, &numOfMonitors);

		ReorderExcessMonitors(monitors, numOfMonitors, client->shape);

		shapes::CenterRectangleOnPlane(Rectangle(), &client->shape);

		delete[] monitors;
	}

}

#endif //FLOW_WM_HANDLER_HELPERS_HPP
