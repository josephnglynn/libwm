//
// Created by joseph on 22/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_KEYBOARD_MANAGER_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_KEYBOARD_MANAGER_HPP

#include <X11/Xlib.h>
#include <vector>
#include "../../general/keybinding.hpp"
#include "../client/client.hpp"



namespace flow
{

	struct KeyboardManager
	{

		KeyboardManager(std::vector<KeyBinding>& kb, std::vector<ClientKeyBinding>& ckb, Key mk);
		void Start(Display* display, Window root_window);
		void Update(std::vector<KeyBinding>& kb, std::vector<ClientKeyBinding>& ckb, Key mk);
		void GrabButtons(X11::Client* client, int focused);


		Key mod_key;
		unsigned int num_lock_mask;
		std::vector<KeyBinding> key_bindings_root;
		std::vector<ClientKeyBinding> key_bindings_client;



		void UpdateNumLockMask(Display* display);
	};
}

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_KEYBOARD_MANAGER_HPP
