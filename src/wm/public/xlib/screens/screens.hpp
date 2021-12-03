//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_SCREENS_HPP
#define FLOW_WM_SCREENS_HPP
#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>
#include <vector>
#include "../../general/shapes.hpp"

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
		ScreenManager();
		void UpdateGeom();
	private:
		std::vector<Monitor>* monitors;
	};

}

#endif //FLOW_WM_SCREENS_HPP
