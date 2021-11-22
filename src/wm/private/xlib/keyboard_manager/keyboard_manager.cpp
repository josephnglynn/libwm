//
// Created by joseph on 22/11/2021.
//

#include "public/xlib/keyboard_manager/keyboard_manager.hpp"
#include <flow_wm_xlib.hpp>

namespace flow
{

	//DWM
	void KeyboardManager::UpdateNumLockMask(Display* display)
	{
		unsigned int i, j;
		XModifierKeymap* modmap;

		num_lock_mask = 0;
		modmap = XGetModifierMapping(display);
		for (i = 0; i < 8; i++)
			for (j = 0; j < modmap->max_keypermod; j++)
				if (modmap->modifiermap[i * modmap->max_keypermod + j] == XKeysymToKeycode(display, num_lock_mask))
					num_lock_mask = (1 << i);
		XFreeModifiermap(modmap);
	}

	KeyboardManager::KeyboardManager(std::vector<KeyBinding>& kb, Key mk) : mod_key(mk)
	{
		for (const auto& item: kb)
		{
			if (item.client)
			{
				key_bindings_client.push_back(item);
			}
			else
			{
				key_bindings_root.push_back(item);
			}
		}
	}

	void KeyboardManager::Start(Display* display, Window root_window)
	{
		UpdateNumLockMask(X11::FlowWindowManagerX11::Get()->GetDisplay());
		unsigned int modifiers[] = { 0, LockMask, num_lock_mask,
									 num_lock_mask | LockMask };
		XUngrabKey(display, AnyKey, AnyModifier, root_window);
		for (unsigned int i = 0; i < key_bindings_root.size(); i++)
		{
			KeyCode keyCode = XKeysymToKeycode(display, key_bindings_root[i].key);
			for (int k = 0; k < sizeof modifiers / sizeof modifiers[0]; ++k) {
				XGrabKey(display, keyCode, key_bindings_root[i].mod_key | modifiers[k], root_window, True, GrabModeAsync, GrabModeAsync);
			}
		}
	}

}
