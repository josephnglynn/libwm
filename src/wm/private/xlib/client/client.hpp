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
				&position.x,
				&position.y,
				&position.width,
				&position.height,
				&border,
				&depth);
		}

		Client* next;
		Client* previous;
		Window window;
		shapes::Rectangle position;
		unsigned int border;
		unsigned int depth;

	};
}

#endif //FLOW_WM_CLIENT_HPP
