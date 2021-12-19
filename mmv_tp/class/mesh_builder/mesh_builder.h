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

	/**
	 * Flat mesh.
	 *
	 * \param width nb points in X
	 * \param height nb points in Z
	 * \param p flat mesh building parametters
	 * \return
	 */
	static Mesh flat_Mesh(int width, int height, params p);

	/**
	 * Flat mesh.
	 *
	 * \param width nb points in X
	 * \param height nb points in Z
	 * \param b Bounding box
	 * \param p flat mesh building parametters
	 * \return
	 */
	static Mesh flat_Mesh(int size_x, int size_y, BBox b, params p);

	/**
	 * Flat mesh.
	 *
	 * \param width nb points in X
	 * \param height nb points in Z
	 * \param b Bounding box
	 * \param funcZ function to compute the height of the flat mesh with the X and Z
	  * \param p flat mesh building parametters
	 * \return
	 */
	static Mesh flat_Mesh(int size_x, int size_y, BBox b, std::function < const float(const int x, const int y)> funcZ, params p);

	/**
	 * Update the height of a flat mesh.
	 *
	 * \param m flat mesh
	 * \param size_x nb points in X
	 * \param size_y nb points in Z
	 * \param b Bounding box
	 * \param funcZ function to compute the height of the flat mesh with the X and Z axes
	 */
	static void Compute_Height(Mesh& m, int size_x, int size_y, BBox b, std::function < const float(const int x, const int y)> funcZ);

	/**
	 * Compute the normal of the flat mesh with an external function.
	 *
	 * \param m flat mesh
	 * \param size_x nb points in X
	 * \param size_y nb points in Z
	 * \param funcZ fonction to compute the normal with the X and Z axes
	 */
	static void Compute_normal(Mesh& m, int size_x, int size_y, std::function < const Vector(const int x, const int y)> funcZ);

	/**
	 * Compute the smoothed normal of a mesh.
	 *
	 * \param m mesh
	 */
	static void Compute_normal(Mesh& m);


};
