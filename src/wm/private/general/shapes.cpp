//
// Created by joseph on 21/11/2021.
//
#include "../../public/general/shapes.hpp"

namespace flow::shapes
{
	void CenterRectangleOnPlane(Rectangle parent, Rectangle* child)
	{
		child->x = parent.x + static_cast<int>((parent.width - child->width) / 2);
		child->y = parent.y + static_cast<int>((parent.height - child->height) / 2);
	}
}