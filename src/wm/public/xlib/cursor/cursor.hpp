//
// Created by joseph on 23/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_CURSOR_CURSOR_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_CURSOR_CURSOR_HPP
#include <X11/Xlib.h>
#include "../drawable_window/drawable_window.hpp"

namespace flow {
	struct Cur {
		Cursor cursor;
	};
}

namespace flow::CursorUtils {
	Cur* CreateCursor(DrawableWindow* display, int shape);
}

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_CURSOR_CURSOR_HPP
