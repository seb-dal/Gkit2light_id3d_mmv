#pragma once

#include "image.h"

class Image_HF {
public:
	Image_HF() {}
	Image_HF(Image img) :img(img) {}

	float operator()(int x, int y) { return img(x, y).r; }
	float interpolate(float x, float y) { return img.sample(x, y).r; }

	size_t height() { return img.height(); }
	size_t width() { return img.width(); }
	size_t size() { return img.size(); }

protected:
	Image img;
};
