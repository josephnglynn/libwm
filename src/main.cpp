//
// Created by joseph on 16/11/2021.
//

#include <wm/public/flow_wm_xlib.hpp>

int main()
{

	auto wm = flow::FlowWindowManagerX11::Init();
	wm->Start();

	return 0;
}
