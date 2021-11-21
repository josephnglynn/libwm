//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_CLIENT_HPP
#define FLOW_WM_CLIENT_HPP
#include <X11/Xlib.h>
#include "../screens/screens.hpp"
#include "flow_wm_xlib.hpp"

namespace flow::X11
{

	struct Client
	{

		Client() = default;

		explicit Client(Window window) : window(window)
		{
			XGetGeometry(FlowWindowManagerX11::Get()->GetDisplay(),
				window,
				nullptr,
				&shape.x,
				&shape.y,
				&shape.width,
				&shape.height,
				&border,
				&depth);
		}

		Client* next;
		Client* previous;
		Window window;
		shapes::Rectangle shape;
		unsigned int border;
		unsigned int depth;

	};
}

#endif //FLOW_WM_CLIENT_HPP
