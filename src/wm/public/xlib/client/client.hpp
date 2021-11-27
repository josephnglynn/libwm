//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_CLIENT_HPP
#define FLOW_WM_CLIENT_HPP
#include "../screens/screens.hpp"


namespace flow::X11
{

	struct Client
	{

		Client() = default;

		explicit Client(Window window);

		void SetUrgent(int urgency);
		void Focus();
		void SetFocus();
		int SendEvent(Atom protocol);
		void UpdateWindowType(Client* client);
		Atom GetAtomProp(Atom prop);
		void SetFullScreen(int fs);
		void ResizeClient(shapes::Rectangle& detail);
		void ResizeClient(int x, int y, int w, int h);
		void Configure();
		void UpdateSizeHints();
		void UpdateWmHints();
		void SetState(long state);
		void GrabButtons(int focused);
		void UnManage(int destroyed);

		Client* next;
		Client* previous;
		Window window;
		shapes::Rectangle position;
		shapes::Rectangle old_position;
		unsigned int border;
		unsigned int depth;
		bool is_urgent;
		bool visible;
		bool never_focus;
		bool full_screen;
		bool configured = false;
	};
}

#endif //FLOW_WM_CLIENT_HPP
