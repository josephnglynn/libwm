//
// Created by joseph on 17/12/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_SHELL_SHELL_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_SHELL_SHELL_HPP
#include <string>
#include <X11/Xlib.h>
#include <functional>
#include "../../../../external/shell-base/src/sbb.hpp"

namespace flow
{

	extern "C" {
		typedef void (* OnLoad_t)();
		typedef BorderInfo (* GetBorderInfo_t)();
		typedef Window (* CreateWindowBorder_t)(int, int, int, int, int);
		typedef Window (* CreateShell_t)(int, int, int, int, int);
	}


	struct Shell
	{
		void* handle{};
		OnLoad_t OnLoad;
		GetBorderInfo_t  GetBorderInfo;
		CreateWindowBorder_t CreateWindowBorder;
		CreateShell_t CreateShell;

		explicit Shell(const std::string& file_name);
		~Shell();
	};
}

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_SHELL_SHELL_HPP
