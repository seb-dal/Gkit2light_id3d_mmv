#include "vecPlus.h"

float m_exp::gaussian2D(float x, float x_0, float y, float y_0, float A, float sigma_x, float sigma_y) {
	float xmx_0 = x - x_0;
	float ymy_0 = y - y_0;
	return
		A * std::exp(
			-(
				(xmx_0 * xmx_0) / (2 * sigma_x * sigma_x)
				+
				(ymy_0 * ymy_0) / (2 * sigma_y * sigma_y)
				)
		);
}




Vector m_exp::perpendicular_Vector_Triangle(const Vector& a, const Vector& b, const Vector& c) {
	return normalize(cross((b - a), (c - a)));
}




float dot(const vec2& u, const vec2& v)
{
	return u.x * v.x + u.y * v.y;
}