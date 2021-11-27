//
// Created by joseph on 27/11/2021.
//

#ifndef FLOW_WM_SRC_WM_PRIVATE_GENERAL_INPUT_FUNCTIONS_HPP
#define FLOW_WM_SRC_WM_PRIVATE_GENERAL_INPUT_FUNCTIONS_HPP
#include <string>
#include <unistd.h>
#include <vector>
#include "../flow_wm_xlib.hpp"




namespace flow::input_functions
{


	std::function<void(const std::string&)> InputFunctionToFunctionPointer(InputFunction i_f);
	char* const* split(std::string s);
	void spawn(const std::string& arg);
}

#endif //FLOW_WM_SRC_WM_PRIVATE_GENERAL_INPUT_FUNCTIONS_HPP
