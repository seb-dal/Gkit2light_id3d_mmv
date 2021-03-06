#pragma once

#include "mesh.h"
#include "vec.h"
#include <functional>

#define uint  int

// free type vec2
template <typename T>
struct vect2 {
	T x, y;
	vect2() :x(), y() {}
	vect2(T v) :x(v), y(v) {}
	vect2(T x, T y) :x(x), y(y) {}
	/*bool operator ==(const vect2& v) {
		if (this == &v)
			return true;
		if (x == v.x && y == v.y)
			return true;
		return false;
	}*/
	bool operator==(const vect2<T>& rhs) const {
		if (this == &rhs)
			return true;
		if (x == rhs.x && y == rhs.y)
			return true;
		return false;
	}

	vect2<T> operator +(const vect2<T>& o) { return vect2<T>(o.x + x, o.y + y); }

};

using Coord2 = vect2<int>;


template <typename T>
struct vect3 {
	T x, y, z;
	vect3() :x(), y(), z() {}
	vect3(T v) :x(v), y(v), z(v) {}
	vect3(T x, T y, T z) :x(x), y(y), z(z) {}
};


namespace std {
	// add clamp to std (not available until c++17)
	template<typename T>
	T clamp(T v, T min, T max) {
		return (min > v) ? min : (v > max) ? max : v;
	}
};




class m_exp {
public:
	static float gaussian2D(float x, float x_0, float y, float y_0, float A, float sigma_x, float sigma_y);


	static Vector perpendicular_Vector_Triangle(const Vector& a, const Vector& b, const Vector& c);
};

float dot(const vec2& u, const vec2& v);




