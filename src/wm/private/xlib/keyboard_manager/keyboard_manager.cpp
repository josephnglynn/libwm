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
		unsigned int i;
		int j;
		XModifierKeymap* modmap;

		num_lock_mask = 0;
		modmap = XGetModifierMapping(display);
		for (i = 0; i < 8; i++)
			for (j = 0; j < modmap->max_keypermod; j++)
				if (modmap->modifiermap[i * modmap->max_keypermod + j] == XKeysymToKeycode(display, num_lock_mask))
					num_lock_mask = (1 << i);
		XFreeModifiermap(modmap);
	}

	KeyboardManager::KeyboardManager(std::vector<KeyBinding>& kb, std::vector<ClientKeyBinding>& ckb, Key mk)
		: key_bindings_client(ckb)
	{
		Update(kb, mk);
	}

	void KeyboardManager::Start(Display* display, Window root_window)
	{
		UpdateNumLockMask(X11::FlowWindowManagerX11::Get()->GetDisplay());
		unsigned int modifiers[] = {
			0,
			LockMask,
			num_lock_mask,
			num_lock_mask | LockMask
		};

		XUngrabKey(display, AnyKey, AnyModifier, root_window);
		for (unsigned int i = 0; i < key_bindings_root.size(); i++)
		{
			KeyCode keyCode = XKeysymToKeycode(display, key_bindings_root[i].key);
			for (unsigned int k = 0; k < sizeof modifiers / sizeof modifiers[0]; ++k)
			{
				XGrabKey(
					display,
					keyCode,
					key_bindings_root[i].mod_key | modifiers[k],
					root_window,
					True,
					GrabModeAsync,
					GrabModeAsync
				);
			}
		}
	}

	void KeyboardManager::Update(std::vector<KeyBinding>& kb, Key mk)
	{
		this->mod_key = mk;
		for (const auto& item: kb)
		{
			key_bindings_root.push_back(item);
		}
	}

	void KeyboardManager::GrabButtons(X11::Client* c, int focused)
	{
		auto fwm = X11::FlowWindowManagerX11::Get();
		UpdateNumLockMask(fwm->GetDisplay());
		unsigned int i, j;
		unsigned int modifiers[] = { 0, LockMask, num_lock_mask, num_lock_mask | LockMask };
		XUngrabButton(fwm->GetDisplay(), AnyButton, AnyModifier, c->window);
		if (!focused)
			XGrabButton(fwm->GetDisplay(), AnyButton, AnyModifier, c->window, False,
				ButtonPressMask | ButtonReleaseMask, GrabModeSync, GrabModeSync, None, None);
		for (i = 0; i < key_bindings_client.size(); i++)
			if (key_bindings_client[i].click == ClkClientWin)
			{
				for (j = 0; j < sizeof modifiers / sizeof modifiers[0]; j++)
					XGrabButton(
						fwm->GetDisplay(), key_bindings_client[i].button,
						key_bindings_client[i].event_mask | modifiers[j],
						c->window, False, ButtonPressMask | ButtonReleaseMask,
						GrabModeAsync, GrabModeSync, None, None
					);
			}

	}

}
