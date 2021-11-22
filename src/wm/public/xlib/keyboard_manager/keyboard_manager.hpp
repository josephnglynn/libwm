//
// Created by joseph on 22/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_KEYBOARD_MANAGER_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_KEYBOARD_MANAGER_HPP
#include <vector>
#include "../../general/keybinding.hpp"
#include <X11/Xlib.h>

namespace flow
{

	class KeyboardManager
	{
	public:
		KeyboardManager(std::vector<KeyBinding>& kb, Key mk);
		void Start(Display* display, Window root_window);
	private:
		Key mod_key;
		unsigned int num_lock_mask;
		std::vector<KeyBinding> key_bindings_root;
		std::vector<KeyBinding> key_bindings_client;

		void UpdateNumLockMask(Display* display);
	};
}

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_KEYBOARD_MANAGER_HPP
