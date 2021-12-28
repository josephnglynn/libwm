//
// Created by joseph on 17/12/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_SHELL_SHELL_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_SHELL_SHELL_HPP
#include <string>
#include <X11/Xlib.h>
#include <functional>
#include "../../../../external/shell-base/src/shell_base.hpp"

namespace flow
{

	extern "C" {
		typedef void (* OnLoad_t)();
		typedef ShellInfo (* GetShellInfo_t)();
		typedef Offsets (* GetOffsets_t)(int);
		typedef Window (*CreateWindow_t)(int, int, int, int, Display*, Window);
		typedef void (*HandleEvent_t)(XEvent*, int, int, int, int);
	}


	struct Shell
	{
		void* handle{};
		OnLoad_t OnLoad;
		GetShellInfo_t  GetShellInfo;
		GetOffsets_t GetOffsets;
		CreateWindow_t CreateWindow;
		HandleEvent_t HandleEvent;

		explicit Shell(const std::string& file_name);
		~Shell();
	};
}

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_SHELL_SHELL_HPP
