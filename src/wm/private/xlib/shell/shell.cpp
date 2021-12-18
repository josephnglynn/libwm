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
		GetBorderInfo = (GetBorderInfo_t) dlsym(handle, "gBI");
		CreateWindowBorder = (CreateWindowBorder_t) dlsym(handle, "cWB");
		CreateShell = (CreateShell_t) dlsym(handle, "cS");

	}

	Shell::~Shell()
	{
		dlclose(handle);
	}
}