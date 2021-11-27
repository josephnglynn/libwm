//
// Created by joseph on 27/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_FNT_FNT_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_FNT_FNT_HPP
#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include "../drawable_window/drawable_window.hpp"

namespace flow {
	struct Fnt
	{
		Display* dpy;
		unsigned int h;
		XftFont* xfont;
		FcPattern* pattern;
		struct Fnt* next;

		static Fnt* XCreateFont(DrawableWindow* drw, const char* fontname, FcPattern* fontpattern);

	};
}

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_FNT_FNT_HPP
