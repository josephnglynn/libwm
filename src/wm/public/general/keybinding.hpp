//
// Created by joseph on 22/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_GENERAL_KEYBINDING_HPP
#define FLOW_WM_SRC_WM_PUBLIC_GENERAL_KEYBINDING_HPP
#include <X11/Xlib.h>
#include <string>
#include <functional>
namespace flow
{

	typedef unsigned int Key;

	enum InputFunction
	{
		Spawn, //Run a command with argument being name of program.
		MoveMouse,
		ResizeMouse,
	};


	//Based on dwm keybinding style
	struct KeyBinding
	{
		KeyBinding() = default;
		KeyBinding(Key mk, KeySym k, InputFunction i_f, std::string arg);

		Key mod_key;
		KeySym key;
		std::function<void(std::string)> function;
		std::string arg;

		friend std::ostream& operator<<(std::ostream& os, const KeyBinding& kb);
	};

	struct ClientKeyBinding
	{
		ClientKeyBinding() = default;
		ClientKeyBinding(int click, int event_mask, int button, InputFunction i_f, std::string arg);

		unsigned int click;
		unsigned int event_mask;
		unsigned int button;
		std::function<void(std::string)> function;
		std::string arg;

		friend std::ostream& operator<<(std::ostream& os, const ClientKeyBinding& kb);
	};
}

#endif //FLOW_WM_SRC_WM_PUBLIC_GENERAL_KEYBINDING_HPP
