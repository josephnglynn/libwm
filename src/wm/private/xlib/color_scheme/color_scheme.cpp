//
// Created by joseph on 27/11/2021.
//

#include "../../../public/xlib/color_scheme/color_scheme.hpp"
#include "../../../../logger/public/logger.hpp"

namespace flow::ColorScheme
{
	XftColor* ScmCreate(DrawableWindow* drw, const char* colour_names[], size_t colour_count)
	{
		size_t i;
		XftColor* ret;

		/* need at least two colors for a scheme */
		if (!drw || !colour_names || colour_count < 2
			|| !(ret = static_cast<XftColor*>(malloc(colour_count * sizeof(XftColor)))))
		{
			return nullptr;
		}

		for (i = 0; i < colour_count; i++)
		{
			ClrCreate(drw, &ret[i], colour_names[i]);
		}

		return ret;
	}

	void ClrCreate(DrawableWindow* drw, XftColor* destination, const char* color_name)
	{
		if (!drw || !destination || !color_name)
			return;

		if (!XftColorAllocName(drw->dpy,
			DefaultVisual(drw->dpy, drw->screen),
			DefaultColormap(drw->dpy, drw->screen),
			color_name,
			destination))
		{
			logger::error("An error occurred allocating colour", color_name);
			std::exit(0);
		}
	}
}