//
// Created by joseph on 16/11/2021.
//

#ifndef FLOW_WM_FLOW_WM_HPP
#define FLOW_WM_FLOW_WM_HPP
#include <X11/Xlib.h>

namespace flow {

	class FlowWindowManager {
	public:
		virtual ~FlowWindowManager() = default;
		virtual void Start() = 0;
	protected:
		bool quit = false;
	};
}

#endif // FLOW_WM_FLOW_WM_HPP
