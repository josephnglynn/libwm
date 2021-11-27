//
// Created by joseph on 27/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_DRAWABLE_WINDOW_DRAWABLE_WINDOW_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_DRAWABLE_WINDOW_DRAWABLE_WINDOW_HPP

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <vector>
#include <string>

namespace flow {
	struct Fnt;

	struct DrawableWindow
	{
		unsigned int w, h;
		Display* dpy;
		int screen;
		Window root;
		Drawable drawable;
		GC gc;
		XftColor* scheme;
		Fnt* fonts;

		static DrawableWindow* Create(Display* display, int screen, Window root, unsigned int w, unsigned int h);
		static Fnt* CreateFontSet(DrawableWindow* drw, std::vector<std::string> fonts);
		void Resize(unsigned int width, unsigned int height);
	};
}

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_DRAWABLE_WINDOW_DRAWABLE_WINDOW_HPP
