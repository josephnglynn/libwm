//
// Created by joseph on 20/11/2021.
//

#ifndef FLOW_WM_TYPES_HPP
#define FLOW_WM_TYPES_HPP

namespace flow::shapes
{

	struct Rectangle
	{
		int x, y;
		unsigned int width, height;

		Rectangle() = default;
		Rectangle(int x, int y, unsigned int w, unsigned int h) : x(x), y(y), width(w), height(h) {}

	};

	void CenterRectangleOnPlane(Rectangle parent, Rectangle* child);

}
#endif //FLOW_WM_TYPES_HPP
