//
// Created by joseph on 17/12/2021.
//

#ifndef FLOW_WM_SRC_WM_PUBLIC_GENERAL_INLINE_FUNCTIONS_HPP
#define FLOW_WM_SRC_WM_PUBLIC_GENERAL_INLINE_FUNCTIONS_HPP

#include "../xlib/client/client.hpp"

namespace flow
{
	inline constexpr int Max(int A, int B)
	{
		return A > B ? A : B;
	}

	inline constexpr int Min(int A, int B)
	{
		return A < B ? A : B;
	}

	inline constexpr int Height(X11::Client* c)
	{
		return c->position.width + 2 * c->border_width;
	}

	inline constexpr int Width(X11::Client* c)
	{
		return c->position.height + 2 * c->border_width;
	}

	inline constexpr int Intersect(int x, int y, int w, int h, Monitor* m)
	{
		return Max(0, Min((x) + (w), (m)->wx + (m)->ww) - Max((x), (m)->wx)) * Max(0, Min((y) + (h), (m)->wy + (m)->wh) - Max((y), (m)->wy));
	}
}

#endif //FLOW_WM_SRC_WM_PUBLIC_GENERAL_INLINE_FUNCTIONS_HPP
