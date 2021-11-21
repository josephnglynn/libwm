//
// Created by joseph on 21/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PRIVATE_XLIB_HANDLERS_HANDLER_HELPERS_HPP
#define FLOW_WM_SRC_WM_PRIVATE_XLIB_HANDLERS_HANDLER_HELPERS_HPP
#include <X11/Xlib.h>
#include "../../general/shapes.hpp"


namespace flow::handler_helpers {
	shapes::Rectangle XConfigureRequestEventToRect(XConfigureRequestEvent xcre) {
		return { xcre.x, xcre.y, static_cast<unsigned int>(xcre.width), static_cast<unsigned int>(xcre.height)};
	}
}


#endif //FLOW_WM_SRC_WM_PRIVATE_XLIB_HANDLERS_HANDLER_HELPERS_HPP
