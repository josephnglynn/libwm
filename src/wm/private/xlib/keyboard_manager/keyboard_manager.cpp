//
// Created by joseph on 22/11/2021.
//

#include "../../../public/xlib/keyboard_manager/keyboard_manager.hpp"
#include "../../../../logger/public/logger.hpp"

namespace flow
{

	//DWM
	void KeyboardManager::UpdateNumLockMask(Display* display)
	{
		unsigned int i;
		int j;
		XModifierKeymap* modmap;

		num_lock_mask = 0;
		modmap = XGetModifierMapping(display);
		for (i = 0; i < 8; i++)
			for (j = 0; j < modmap->max_keypermod; j++)
				if (modmap->modifiermap[i * modmap->max_keypermod + j] == XKeysymToKeycode(display, XK_Num_Lock))
					num_lock_mask = (1 << i);
		XFreeModifiermap(modmap);
	}

	KeyboardManager::KeyboardManager(std::vector<KeyBinding>& kb, std::vector<ClientKeyBinding>& ckb, Key mk)
	{
		Update(kb, ckb, mk);
	}

	void KeyboardManager::Start(Display* display, Window root_window)
	{
		UpdateNumLockMask(display);
		unsigned int modifiers[] = {
			0,
			LockMask,
			num_lock_mask,
			num_lock_mask | LockMask
		};



		KeyCode key_code;
		XUngrabKey(display, AnyKey, AnyModifier, root_window);

		for (unsigned int i = 0; i < key_bindings_root.size(); i++)
		{
			if ((key_code = XKeysymToKeycode(display, key_bindings_root[i].key)))
			{
				for (unsigned int k = 0; k < sizeof modifiers / sizeof modifiers[0]; ++k)
				{

					XGrabKey(
 						display,
						key_code,
						key_bindings_root[i].mod_key | modifiers[k],
						root_window,
						True,
						GrabModeAsync,
						GrabModeAsync
					);
				}
			}
		}
	}





	void KeyboardManager::Update(std::vector<KeyBinding>& kb, std::vector<ClientKeyBinding>& ckb, Key mk)
	{
		this->mod_key = mk;
		key_bindings_root = kb;
		key_bindings_client = ckb;
	}

}
