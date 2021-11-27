//
// Created by joseph on 27/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_COLOR_SCHEME_COLOR_SCHEME_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_COLOR_SCHEME_COLOR_SCHEME_HPP

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include "public/xlib/drawable_window/drawable_window.hpp"


namespace flow::ColorScheme
{
	XftColor* ScmCreate(DrawableWindow* drw, const char* colour_names[], size_t colour_count);
	void ClrCreate(DrawableWindow* drw, XftColor* destination, const char* color_name);
}

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_COLOR_SCHEME_COLOR_SCHEME_HPP
