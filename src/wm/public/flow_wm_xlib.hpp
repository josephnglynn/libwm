//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_FLOW_WM_XLIB_HPP
#define FLOW_WM_FLOW_WM_XLIB_HPP
#include <X11/Xlib.h>

namespace flow
{

	int FlowX11ErrorHandler(Display* display, XErrorEvent* event);

	class FlowWindowManagerX11
	{
	public:
		~FlowWindowManagerX11();
		static FlowWindowManagerX11* Init();
		static FlowWindowManagerX11* Get();
		void Start();
		Display* GetDisplay();
	private:
		Display* display = nullptr;
		Window rootWindow = 0;
		bool quit = false;
		static FlowWindowManagerX11* instance;
	};
}

#endif //FLOW_WM_FLOW_WM_XLIB_HPP
