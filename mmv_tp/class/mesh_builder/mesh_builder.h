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

		bool mat;
		Color c;
		// n:normal, t:texcoord 
		params(bool n, bool t) :normal(n), textcoord(t), mat(false) {}
		params(bool n, bool t, Color c) :normal(n), textcoord(t), mat(true), c(c) {}
	};

	static Mesh flat_Mesh(int width, int height, params p);

	static Mesh flat_Mesh(int size_x, int size_y, BBox b, params p);

	static Mesh flat_Mesh(int size_x, int size_y, BBox b, std::function < const float(const int x, const int y)> funcZ, params p);


	static void Compute_Height(Mesh& m, int size_x, int size_y, BBox b, std::function < const float(const int x, const int y)> funcZ);

	static void Compute_normal(Mesh& m, int size_x, int size_y, std::function < const Vector(const int x, const int y)> funcZ);;
	static void Compute_normal(Mesh& m);


};
