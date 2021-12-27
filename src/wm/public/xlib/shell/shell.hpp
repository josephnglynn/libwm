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
		typedef BorderInfo (* GetBorderInfo_t)();
		typedef Offsets (* GetOffsets_t)(int);
		typedef Window (*CreateWindow_t)(int, int, int, int, Display*, Window);
		typedef void (*RunWindow_t)(Window, Display*, Window);
	}


	struct Shell
	{
		void* handle{};
		OnLoad_t OnLoad;
		GetBorderInfo_t  GetBorderInfo;
		GetOffsets_t GetOffsets;
		CreateWindow_t CreateWindow;
		RunWindow_t RunWindow;

		explicit Shell(const std::string& file_name);
		~Shell();
	};
}

#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_SHELL_SHELL_HPP
