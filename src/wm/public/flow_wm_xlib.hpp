//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_FLOW_WM_XLIB_HPP
#define FLOW_WM_FLOW_WM_XLIB_HPP
#include <X11/Xlib.h>
#include "flow_wm.hpp"

namespace flow {

	int FlowX11ErrorHandler(Display* display, XErrorEvent* event);

	class FlowWindowManagerX11 : FlowWindowManager {
	public:
		~FlowWindowManagerX11() override;
		void Start() override;
		Display* getDisplay() { return display; }
	private:
		Display* display;
		Window rootWindow;
	};
}


#endif //FLOW_WM_FLOW_WM_XLIB_HPP
