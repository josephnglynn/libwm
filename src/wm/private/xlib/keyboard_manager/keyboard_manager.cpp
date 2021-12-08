//
// Created by joseph on 22/11/2021.
//

#include "../../../public/xlib/keyboard_manager/keyboard_manager.hpp"
#include <X11/keysym.h>
#include <flow_wm_xlib.hpp>

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
	void KeyboardManager::GrabButtons(X11::Client* client, int focused)
	{
		if (!client) return;
		auto fwm = X11::FlowWindowManagerX11::Get();
		auto km = fwm->GetKeyboardManager();
		km->UpdateNumLockMask(fwm->GetDisplay());
		unsigned int i, j;
		unsigned int modifiers[] = {
			0,
			LockMask,
			km->num_lock_mask,
			km->num_lock_mask | LockMask
		};
		XUngrabButton(fwm->GetDisplay(), AnyButton, AnyModifier, client-> window);
		if (!focused)
		{
			XGrabButton(
				fwm->GetDisplay(),
				AnyButton,
				AnyModifier,
				client->window,
				False,
				ButtonPressMask | ButtonReleaseMask,
				GrabModeSync,
				GrabModeSync,
				None,
				None
			);
		}
		for (i = 0; i < km->key_bindings_client.size(); i++)
		{
			if (km->key_bindings_client[i].click == ClkClientWin)
			{
				for (j = 0; j < sizeof modifiers / sizeof modifiers[0]; j++)
					XGrabButton(
						fwm->GetDisplay(), km->key_bindings_client[i].button,
						km->key_bindings_client[i].event_mask | modifiers[j],
						client->window, False, ButtonPressMask | ButtonReleaseMask,
						GrabModeAsync, GrabModeSync, None, None
					);
			}
		}

	}
}
