#pragma once
#include "vec.h"

struct BBox {
	Point pmin, pmax;

	BBox() {}

	BBox(Point& pm, Point& pM) :pmin(pm), pmax(pM) {};

};

