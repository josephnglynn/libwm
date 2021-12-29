//
// Created by joseph on 17/12/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_SHELL_SHELL_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_SHELL_SHELL_HPP
#include <string>
#include <X11/Xlib.h>
#include <functional>
#include "../../../../external/shell-base/src/types/types.hpp"

namespace flow
{

	extern "C" {
		typedef void (* OnLoad_t)();
		typedef ShellInfo (* GetShellInfo_t)();
		typedef Offsets (* GetOffsets_t)(int);
		typedef Window (*CreateWindow_t)(int, int, int, int, Display*, Window);
		typedef void (*HandleEventFrame_t)(XEvent*, int, int, int, int);
		typedef Window (*CreateBackWindow_t)(int, int, int, int, Display*, Window);
		typedef void (*HandleEventBase_t)(XEvent*, int, int, int, int);
		typedef NonConformingWB (*GetNCWB_t)();
	}


	struct Shell
	{
		void* handle{};
		OnLoad_t OnLoad;
		GetShellInfo_t  GetShellInfo;
		GetOffsets_t GetOffsets;
		CreateWindow_t CreateWindow;
		HandleEventFrame_t HandleEventFrame;
		CreateBackWindow_t CreateBackWindow;
		HandleEventBase_t HandleEventBase;
		GetNCWB_t GetNCWB;

		explicit Shell(const std::string& file_name);
		~Shell();
	};
}

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_SHELL_SHELL_HPP
