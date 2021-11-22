//
// Created by joseph on 22/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_KEYBOARD_MANAGER_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_KEYBOARD_MANAGER_HPP
#include <vector>
#include "../../general/keybinding.hpp"


namespace flow
{

	class KeyboardManager
	{
	public:
		KeyboardManager(KeyBinding* kb, Key mk,int num);
	private:
		int num;
		Key modKey;
		KeyBinding* keybindings;
	};
}

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_KEYBOARD_MANAGER_HPP
