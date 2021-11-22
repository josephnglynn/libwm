//
// Created by joseph on 22/11/2021.
//

#include "public/general/keybinding.hpp"
#include <iostream>
#include <utility>

namespace flow
{

	KeyBinding::KeyBinding(Key mk, KeySym k, KeyFunction kf, std::string  arg, bool client)
		: mod_key(mk), key(k), function(kf), arg(std::move(arg)), client(client)
	{

	}
	std::ostream& operator<<(std::ostream& os, const KeyBinding& kb)
	{
		os << "modkey: "<< kb.mod_key << std::endl
				  << "key: " << kb.key << std::endl
				  << "function: " << kb.function << std::endl
				  << "args: " << kb.arg << std::endl;
		return os;
	}
}