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
			instance->AddMonitor(monitor);
#ifdef DEBUG
			logger::info("ADDING SCREEN: ", "Width:", result[i].width, "Height:", result[i].height);
#endif
		}

	}

	void ScreenManager::AddMonitor(Monitor* monitor)
	{
		if (!first)
		{
			first = monitor;
			last = monitor;
		}

		if (last) last->next = monitor;
		last = monitor;
	}

	Monitor* ScreenManager::GetMonitor(Display* display, Window window, int* num)
	{
		int numOfScreens;
		auto result = XRRGetMonitors(display, window, true, &numOfScreens);

		auto* monitors = new Monitor[numOfScreens];
		for (int i = 0; i < numOfScreens; ++i)
		{
			monitors[numOfScreens] = Monitor(result[i]);
		}

		return monitors;
	}

	shapes::Rectangle Monitor::toRectangle()
	{
		return { x, y, static_cast<unsigned int>(width), static_cast<unsigned int>(height) };
	}
}