//
// Created by joseph on 27/11/2021.
//

#include "public/xlib/fnt/fnt.hpp"
#include "../../../../logger/public/logger.hpp"

namespace flow {
	Fnt* Fnt::XCreateFont(DrawableWindow* drw, const char* fontname, FcPattern* fontpattern)
	{
		{
			Fnt* font;
			XftFont* xfont = nullptr;
			FcPattern* pattern = nullptr;

			if (fontname)
			{
				if (!(xfont = XftFontOpenName(drw->dpy, drw->screen, fontname)))
				{
					logger::error("error, cannot load font from name: ", fontname);
					return nullptr;
				}
				if (!(pattern = FcNameParse((FcChar8*)fontname)))
				{
					logger::error("error, cannot parse font name to pattern: ", fontname);
					XftFontClose(drw->dpy, xfont);
					return nullptr;
				}
			}
			else if (fontpattern)
			{
				if (!(xfont = XftFontOpenPattern(drw->dpy, fontpattern)))
				{
					logger::error("error, cannot load font from pattern.");
					return nullptr;
				}
			}
			else
			{
				logger::error("No font specified");
				std::exit(-1);
			}

			FcBool is_col;
			if (FcPatternGetBool(xfont->pattern, FC_COLOR, 0, &is_col) == FcResultMatch && is_col)
			{
				XftFontClose(drw->dpy, xfont);
				return nullptr;
			}

			font = static_cast<Fnt*>(malloc(sizeof(Fnt)));
			font->xfont = xfont;
			font->pattern = pattern;
			font->h = xfont->ascent + xfont->descent;
			font->dpy = drw->dpy;

			return font;
		}
	}
}