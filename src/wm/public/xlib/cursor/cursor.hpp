//
// Created by joseph on 23/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_CURSOR_CURSOR_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_CURSOR_CURSOR_HPP
#include <X11/Xlib.h>

namespace flow::CursorUtils {
	Cursor* CreateCursor() {
		Cursor* cursor = XCreateFontCursor();

	}
}

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_CURSOR_CURSOR_HPP
