//
// Created by joseph on 22/11/2021.
//

#include "../../../public/xlib/keyboard_manager/keyboard_manager.hpp"

namespace flow
{
	KeyboardManager::KeyboardManager(flow::KeyBinding* kb, Key mk, int num) : num(num), modKey(mk), keybindings(kb)
	{
	}

}
