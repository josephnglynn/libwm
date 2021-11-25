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
		void FocusClient();
		void SetFocus();
		int SendEvent(Atom protocol);

		Client* next;
		Client* previous;
		Window window;
		shapes::Rectangle position;
		unsigned int border;
		unsigned int depth;
		bool is_urgent;
		bool visible;
		bool never_focus;

	};
}

#endif //FLOW_WM_CLIENT_HPP
