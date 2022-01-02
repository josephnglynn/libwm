//
// Created by joseph on 17/12/2021.
//

#include "../../../public/xlib/shell/shell.hpp"
#include <dlfcn.h>
#include "../../logger/public/logger.hpp"

namespace flow
{

	Shell::Shell(const std::string& file_location)
	{
		handle = dlopen(file_location.c_str(), RTLD_NOW);

		if (!handle)
		{
			logger::error("Something went wrong loading the shell: ", dlerror());
		}

		OnLoad = (OnLoad_t) dlsym(handle, "oL");
		GetShellInfo = (GetShellInfo_t) dlsym(handle, "gBI");
		GetOffsets = (GetOffsets_t) dlsym(handle, "gOWA");
		CreateFrame = (CreateFrame_t) dlsym(handle, "cF");
		HandleEventFrame = (HandleEventFrame_t) dlsym(handle, "hEF");
		CreateBackWindow = (CreateBackWindow_t) dlsym(handle, "cBW");
		HandleEventBase = (HandleEventBase_t) dlsym(handle, "hEB");
	}

	Shell::~Shell()
	{
		dlclose(handle);
	}
}