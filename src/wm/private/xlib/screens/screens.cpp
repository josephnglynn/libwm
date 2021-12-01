//
// Created by joseph on 20/11/2021.
//

#include "public/xlib/screens/screens.hpp"
#include "../../../../logger/public/logger.hpp"
#include "public/xlib/client/client.hpp"
#include "public/flow_wm_xlib.hpp"

namespace flow
{

	using namespace X11;

	ScreenManager::ScreenManager()
	{
		monitors = new std::vector<Monitor>();
	}

	void ScreenManager::UpdateGeom()
	{
		auto fwm = FlowWindowManagerX11::Get();


		{
			int numOfScreens;
			auto result = XRRGetMonitors(fwm->GetDisplay(), fwm->GetRootWindow(), true, &numOfScreens);

			if (numOfScreens == monitors->size()) return;
			{
				delete monitors;
				monitors = new std::vector<Monitor>(numOfScreens);
			}


			for (int i = 0; i < numOfScreens; i++)
			{
				monitors->push_back(Monitor(result[i]));
				logger::info("ADDING SCREEN: ", "Width:", result[i].width, "Height:", result[i].height);
			}

		}
	}

	shapes::Rectangle Monitor::toRectangle()
	{
		return { x, y, static_cast<unsigned int>(width), static_cast<unsigned int>(height) };
	}
}