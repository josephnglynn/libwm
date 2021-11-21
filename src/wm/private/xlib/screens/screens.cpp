//
// Created by joseph on 20/11/2021.
//

#include "screens.hpp"
#include "../../../../logger/public/logger.hpp"

namespace flow
{
	ScreenManager* ScreenManager::instance;

	void ScreenManager::Init(Display* display, Window rootWindow)
	{
		instance = new ScreenManager();

		int numOfScreens;
		auto result = XRRGetMonitors(display, rootWindow, true, &numOfScreens);

		for (int i = 0; i < numOfScreens; i++)
		{
			auto* monitor = new Monitor(result[i]);
			instance->monitors.push_back(monitor);
			logger::info("ADDING SCREEN: ", "Width:", result[i].width, "Height:", result[i].height);

		}

	}
	ScreenManager* ScreenManager::Get()
	{
		return instance;
	}

	Monitor* ScreenManager::GetClientMonitor(shapes::Rectangle windowPos)
	{
		for (auto i = instance->monitors.size() - 1; i >= 0; --i)
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