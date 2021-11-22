//
// Created by joseph on 22/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_GENERAL_KEYBINDING_HPP
#define FLOW_WM_SRC_WM_PUBLIC_GENERAL_KEYBINDING_HPP


namespace flow
{

	typedef int Key;

	enum KeyFunction
	{
		Spawn, //Run a command with argument being name of program.
		//Other things to do on button press
	};

	struct KeyFunctionArg
	{
		int size;
		void* data;
	};

	//Based on dwm keybinding style
	struct KeyBinding
	{
		Key modKey;
		Key key;
		KeyFunction function;
		KeyFunctionArg arg;
	};
}


#endif //FLOW_WM_SRC_WM_PUBLIC_GENERAL_KEYBINDING_HPP
