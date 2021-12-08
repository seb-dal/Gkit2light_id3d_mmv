#pragma once

#include <functional>

#include "mesh.h"
#include "../BBox/BBox.h"
#include "../vecPlus/vecPlus.h"
#include <omp.h>

class Builder {
public:
	struct params {
		bool normal;
		bool textcoord;

		// n:normal, t:texcoord 
		params(bool n, bool t) :normal(n), textcoord(t) {}
	};

	static Mesh flat_Mesh(int height, int width, params p);

	static Mesh flat_Mesh(int size_y, int size_x, BBox b, params p);

	static Mesh flat_Mesh(int size_y, int size_x, BBox b, std::function < const float(const int x, const int y)> funcZ, params p);


	static void Compute_Height(Mesh& m, int size_y, int size_x, BBox b, std::function < const float(const int x, const int y)> funcZ);

	static void Compute_normal(Mesh& m, int size_y, int size_x, std::function < const Vector(const int x, const int y)> funcZ);;
	static void Compute_normal(Mesh& m);
};
