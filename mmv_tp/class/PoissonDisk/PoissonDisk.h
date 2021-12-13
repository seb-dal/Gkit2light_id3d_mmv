#pragma once

#include <cmath>
#include <vector>

static const double to_rad = M_PI / 180.f;
static float rad(const float deg)
{
	return to_rad * deg;
}

static float rand_float(float High = 1, float Low = 0) {
	return Low + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (High - Low)));
}


class PoissonDisk {
	const std::pair<float, float> null = std::make_pair(-1, -1);

	static float dist(const std::pair<float, float>& a, const std::pair<float, float>& b) {
		float dx = a.first - b.first;
		float dy = a.second - b.second;
		return std::sqrtf(dx * dx + dy * dy);
	}



	void insertPoint(const std::pair<float, float>& point) {
		int xindex = floor(point.first / cellsize);
		int yindex = floor(point.second / cellsize);
		grid[xindex + yindex * ncells_width] = point;
	}


	bool isValidPoint(const std::pair<float, float>& p) {
		/* Make sure the point is on the screen */
		if (p.first < 0 || p.first >= width || p.second < 0 || p.second >= height)
			return false;

		/* Check neighboring eight cells */
		int xindex = std::floor(p.first / cellsize);
		int yindex = floor(p.second / cellsize);
		int i0 = std::max(xindex - 1, 0);
		int i1 = std::min(xindex + 1, ncells_width - 1);
		int j0 = std::max(yindex - 1, 0);
		int j1 = std::min(yindex + 1, ncells_height - 1);

		for (int i = i0; i <= i1; i++)
			for (int j = j0; j <= j1; j++) {
				const std::pair<float, float> pp = grid[i + j * ncells_width];
				if (pp.first != null.first && pp.second != null.second)
					if (dist(pp, p) < radius)
						return false;
			}

		/* If we get here, return true */
		return true;
	}

public:
	/**
	 * Create a 2D PoissonDisk distribution.
	 *
	 * \param size size of the map
	 * \param radius space taken by a point
	 * \param k number of tries to find a new points in the neiborhood
	 */
	PoissonDisk(std::pair<int, int>& size, float radius, int k) :k(k), radius(radius) {
		width = size.first;
		height = size.second;

		cellsize = std::max(std::floorf(radius / std::sqrtf(N)), 1.f);

		ncells_width = ceil(width / cellsize) + 1;
		ncells_height = ceil(height / cellsize) + 1;

		grid.resize(ncells_width * ncells_height);

		for (int i = 0; i < ncells_width; i++) {
			for (int j = 0; j < ncells_height; j++) {
				grid.at(i + j * ncells_width) = null;
			}
		}
	}



	void add_point(const std::pair<float, float>& p) {
		insertPoint(p);
		points.push_back(p);
		active.push_back(p);
	}

	/**
	 * Execute the computing of PoissonDisk.
	 *
	 * \return list of points
	 */
	std::vector<std::pair<float, float>> poissonDiskSampling() {
		if (active.empty()) {
			std::pair<float, float> p0 = std::make_pair(rand_float(width), rand_float(height));
			add_point(p0);
		}


		while (!active.empty()) {
			int random_index = rand() % int(active.size());
			std::pair<float, float> p = active.at(random_index);

			bool found = false;
			for (int tries = 0; tries < k; tries++) {
				float theta = rand_float(360);
				float new_radius = rand_float(2 * radius, radius);
				float pnewx = p.first + new_radius * std::cos(rad(theta));
				float pnewy = p.second + new_radius * std::sin(rad(theta));

				std::pair<float, float> pnew = std::make_pair(pnewx, pnewy);
				if (!isValidPoint(pnew)) {
					continue;
				}

				add_point(pnew);

				found = true;
				break;
			}
			if (!found)
				active.erase(active.begin() + random_index);
		}

		return points;
	}



public:
	std::vector<std::pair<float, float>> points, active;

	int N = 2;
	int width, height, ncells_width, ncells_height, k;
	std::vector<std::pair<float, float>> grid;

	float cellsize, radius;
};
