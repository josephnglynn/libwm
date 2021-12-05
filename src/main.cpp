//
// Created by joseph on 16/11/2021.
//

#include <wm/public/flow_wm_xlib.hpp>
#include <unistd.h>

int main()
{
	auto config = new flow::Config("config.json");
	auto wm = flow::X11::FlowWindowManagerX11::Init(config);
	wm->Start();

	return 0;
}
