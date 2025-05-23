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
	int GetRootPointer(int* x, int* y);
	void spawn(const std::string& arg);
	void moveMouse(const std::string& arg);
	void resizeMouse(const std::string& arg);
	void killClient(const std::string& arg);
	void focusClient(const std::string& arg);
	void reloadShell(const std::string& arg);
	void fullScreen(const std::string& arg);
}

#endif //FLOW_WM_SRC_WM_PRIVATE_GENERAL_INPUT_FUNCTIONS_HPP
