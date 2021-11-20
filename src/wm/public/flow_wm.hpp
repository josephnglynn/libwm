//
// Created by joseph on 16/11/2021.
//

#ifndef FLOW_WM_FLOW_WM_HPP
#define FLOW_WM_FLOW_WM_HPP
#include <X11/Xlib.h>

namespace flow {

	class FlowWindowManager {
	public:
		FlowWindowManager();
		~FlowWindowManager();
		void Start();
		static int FlowX11ErrorHandler(Display* display, XErrorEvent* event);

		Display* getDisplay() { return display; }
	private:
		Display* display;
		Window rootWindow;
		bool quit;
	};
}

#endif // FLOW_WM_FLOW_WM_HPP
