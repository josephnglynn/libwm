//
// Created by joseph on 22/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_GENERAL_KEYBINDING_HPP
#define FLOW_WM_SRC_WM_PUBLIC_GENERAL_KEYBINDING_HPP
#include <X11/Xlib.h>
#include <string>
namespace flow
{

	typedef unsigned int Key;

	enum KeyFunction
	{
		Spawn, //Run a command with argument being name of program.
		//Other things to do on button press
	};

	//Based on dwm keybinding style
	struct KeyBinding
	{
		KeyBinding() = default;
		KeyBinding(Key mk, KeySym k, KeyFunction kf, std::string arg, bool client);

		bool client;
		Key mod_key;
		KeySym key;
		KeyFunction function;
		std::string arg;

		friend std::ostream& operator<<(std::ostream& os, const KeyBinding& kb);
	};
}

#endif //FLOW_WM_SRC_WM_PUBLIC_GENERAL_KEYBINDING_HPP
