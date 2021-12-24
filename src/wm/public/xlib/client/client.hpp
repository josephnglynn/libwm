//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_CLIENT_HPP
#define FLOW_WM_CLIENT_HPP
#include <X11/Xft/Xft.h>
#include "../screens/screens.hpp"
#include <string>
#include "../../../../external/shell-base/src/shell_base.hpp"

namespace flow::X11
{

	struct Client
	{

		Client() = default;
		explicit Client(Window window);

		void SetUrgent(int urgency);
		void SetFocus();
		int SendEvent(Atom protocol);
		void UpdateWindowType();
		Atom GetAtomProp(Atom prop);
		void SetFullScreen(int fs);
		void ResizeClient(shapes::Rectangle& detail);
		void ResizeClient(int x, int y, int w, int h);
		void Configure();
		void UpdateSizeHints();
		void UpdateWmHints();
		void SetState(long state);
		int ApplySizeHints(int* x, int* y, int* w, int* h, int interact);
		void SendMonitor(Monitor* m);
		void UpdateFrame() const;
		void Ban();
		void UnBan();
		void UpdateTitle();

		Client* next;
		Client* previous;
		Window window;
		float min_a, max_a;
		int base_width, base_height, inc_width, inc_height, min_width, min_height, max_width, max_height, border_width, old_border_width;
		shapes::Rectangle position;
		shapes::Rectangle old_position;
		unsigned int depth;
		bool is_urgent;
		bool visible;
		bool never_focus;
		bool full_screen;
		bool configured = false;
		Monitor* monitor;
		Window framer;
		Window base;
		Pixmap drawable;
		XftDraw *xft_draw;
		long flags;
		bool is_annoying;
		int banned;
		int ignore_un_map;
		std::string name;
		bool is_icon;
		Offsets frame_offsets;
	};
}

#endif //FLOW_WM_CLIENT_HPP
