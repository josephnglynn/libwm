//
// Created by joseph on 20/11/2021.
//

#include "public/xlib/screens/screens.hpp"
#include "../../../../logger/public/logger.hpp"

namespace flow
{


	ScreenManager::ScreenManager(Display* display, Window rootWindow)
	{


		int numOfScreens;
		auto result = XRRGetMonitors(display, rootWindow, true, &numOfScreens);

		for (int i = 0; i < numOfScreens; i++)
		{
			auto* monitor = new Monitor(result[i]);
			monitors.push_back(monitor);
			logger::info("ADDING SCREEN: ", "Width:", result[i].width, "Height:", result[i].height);

		}

	}


	Monitor* ScreenManager::GetClientMonitor(shapes::Rectangle windowPos)
	{
		for (auto i = monitors.size() - 1; i >= 0; --i)
		{
			if (monitors[i]->x <= windowPos.x && monitors[i]->y <= windowPos.y)
			{
				return monitors[i];
			}
		}

		return nullptr;
	}

	shapes::Rectangle Monitor::toRectangle()
	{
		return { x, y, static_cast<unsigned int>(width), static_cast<unsigned int>(height) };
	}
}