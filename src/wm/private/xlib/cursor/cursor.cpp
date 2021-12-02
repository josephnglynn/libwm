//
// Created by joseph on 23/11/2021.
//

#include "xlib/cursor/cursor.hpp"


namespace flow::CursorUtils
{
	Cur* CreateCursor(DrawableWindow* drw, int shape)
	{
		Cur* cur = new Cur();
		cur->cursor = XCreateFontCursor(drw->dpy, shape);

		return cur;
	}
}