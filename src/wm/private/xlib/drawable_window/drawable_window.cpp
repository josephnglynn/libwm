//
// Created by joseph on 27/11/2021.
//

#include "../../../public/xlib/drawable_window/drawable_window.hpp"
#include "../../../public/xlib/fnt/fnt.hpp"

namespace flow {
	DrawableWindow* DrawableWindow::Create(Display* display, int screen, Window root, unsigned int w, unsigned int h)
	{
		auto drw = new DrawableWindow();

		drw->dpy = display;
		drw->screen = screen;
		drw->root = root;
		drw->w = w;
		drw->h = h;
		drw->drawable = XCreatePixmap(display, root, w, h, DefaultDepth(display, screen));
		drw->gc = XCreateGC(display, root, 0, nullptr);
		XSetLineAttributes(display, drw->gc, 1, LineSolid, CapButt, JoinMiter);

		return drw;
	}

	Fnt* DrawableWindow::CreateFontSet(DrawableWindow* drw, std::vector<std::string> fonts)
	{
		Fnt* cur, *ret = nullptr;
		size_t i;

		if (!drw || fonts.empty())
			return nullptr;

		const unsigned long count = fonts.size();
		for (i = 1; i <= count; i++)
		{
			if ((cur = Fnt::XCreateFont(drw, fonts[count - 1].c_str(), nullptr)))
			{
				cur->next = ret;
				ret = cur;
			}
		}
		return (drw->fonts = ret);
	}


	void DrawableWindow::Resize(unsigned int width, unsigned int height)
	{
		w = width;
		h = height;
		if (drawable) {
			XFreePixmap(dpy, drawable);
		}
		drawable = XCreatePixmap(dpy, root, w, h, DefaultDepth(dpy, screen));
	}
}