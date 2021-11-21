//
// Created by joseph on 20/11/2021.
//

#include "public/xlib/client/client.hpp"
#include "public/flow_wm_xlib.hpp"
#include <X11/Xlib.h>

flow::X11::Client::Client(Window window)  : window(window)
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
