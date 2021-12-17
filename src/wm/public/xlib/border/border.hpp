//
// Created by joseph on 17/12/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_XLIB_BORDER_BORDER_HPP
#define FLOW_WM_SRC_WM_PUBLIC_XLIB_BORDER_BORDER_HPP
#include <X11/Xlib.h>

//TODO: Make sure to update this file in the other repositories
namespace flow {


	class Border {
	public:
		virtual ~Border();
		virtual Window CreateWindow();

		static void OpenDll() {

		}
	};

}


#endif //FLOW_WM_SRC_WM_PUBLIC_XLIB_BORDER_BORDER_HPP
