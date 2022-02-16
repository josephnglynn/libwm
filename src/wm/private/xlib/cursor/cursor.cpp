//
// Created by joseph on 23/11/2021.
//

#include "../../../public/xlib/cursor/cursor.hpp"


namespace flow::CursorUtils
{
	Cur* CreateCursor(Display* display, int shape)
	{
		Cur* cur = new Cur();
		cur->cursor = XCreateFontCursor(display, shape);

		return cur;
	}
}