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

		Client* next;
		Client* previous;
		Window window;
		shapes::Rectangle position;
		unsigned int border;
		unsigned int depth;

	};
}

#endif //FLOW_WM_CLIENT_HPP
