//
// Created by joseph on 22/11/2021.
//

#include <iostream>
#include <utility>
#include "../../public/general/input_functions.hpp"

#define SPREAD "    "

namespace flow
{


	KeyBinding::KeyBinding(Key mk, KeySym k, InputFunction i_f, std::string  arg)
		:  mod_key(mk), key(k),  arg(std::move(arg))
	{
		function = input_functions::InputFunctionToFunctionPointer(i_f);
	}


	std::ostream& operator<<(std::ostream& os, const KeyBinding& kb)
	{
		os << "modkey: "<< kb.mod_key << "\n"  << SPREAD
				  << "key: " << kb.key << "\n"  << SPREAD
			//	  << "function: " << kb.function << std::endl
				  << "args: " << kb.arg << std::endl;
		return os;
	}

	ClientKeyBinding::ClientKeyBinding(int click, int event_mask, int button, InputFunction i_f, std::string  arg) : click(click), event_mask(event_mask), button(button), arg(std::move(arg))
	{
		function =  input_functions::InputFunctionToFunctionPointer(i_f);
	}

	std::ostream& operator<<(std::ostream& os, const ClientKeyBinding& ckb)
	{
		os << "click: "<< ckb.click << "\n" << SPREAD
		   << "event_mask: " << ckb.event_mask << "\n" << SPREAD
		   << "button: " << ckb.button << "\n" << SPREAD
	//	   << "function: " << ckb.function << std::endl
		   << "args: " << ckb.arg << std::endl;
		return os;
	}



}