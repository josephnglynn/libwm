//
// Created by joseph on 16/11/2021.
//

#include <wm/public/flow_wm_xlib.hpp>

int main()
{

	auto wm = flow::X11::FlowWindowManagerX11::Init();
	wm->SetConfig(new flow::Config("config.json"));
	wm->Start();

	return 0;
}
