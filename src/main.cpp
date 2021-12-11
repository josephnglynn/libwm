//
// Created by joseph on 16/11/2021.
//

#include <wm/public/flow_wm_xlib.hpp>

int main()
{
	auto config = flow::Config::FromFilePath("config.json"); //flow::Config::GetDefault();
	auto wm = flow::X11::FlowWindowManagerX11::Init(config);
	wm->Start();

	return 0;
}
