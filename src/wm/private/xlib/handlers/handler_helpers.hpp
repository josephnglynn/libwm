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

	void CenterChild(Display* display, X11::Client* client)
	{
		Rectangle rectangle = Rectangle();
		int numOfMonitors;
		auto monitors = ScreenManager::GetMonitor(display, client->window, &numOfMonitors);
		Monitor m;

		if (numOfMonitors > 1 && (monitors[0].width - (static_cast<int>(client->shape.width) / 2) > client->shape.x))
		{
			monitors[0] = monitors[1];
		}

		shapes::CenterRectangleOnPlane(Rectangle(), &client->shape);

		delete[] monitors;
	}

}

#endif //FLOW_WM_HANDLER_HELPERS_HPP
