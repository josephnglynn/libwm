//
// Created by joseph on 17/12/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_BORDER_BORDER_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_BORDER_BORDER_HPP
#include <X11/Xlib.h>

namespace flow {

	class Border {
	public:
		virtual Window CreateWindow();
	};

}


#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_BORDER_BORDER_HPP
