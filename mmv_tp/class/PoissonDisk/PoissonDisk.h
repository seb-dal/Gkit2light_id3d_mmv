#pragma once

#include <cmath>
#include <vector>

#include "../vecPlus/Utility.h"
#include <src/gKit/mat.h>




class PoissonDisk {
	const std::pair<float, float> null = std::make_pair(-1, -1);

	static float dist(const std::pair<float, float>& a, const std::pair<float, float>& b) {
		float dx = a.first - b.first;
		float dy = a.second - b.second;
		return std::sqrtf(dx * dx + dy * dy);
	}

public:
	PoissonDisk(std::pair<int, int>& size) {
		width = size.first;
		height = size.second;
	}

	bool isValidPoint(std::pair<float, float>& p, float radius) {
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



	void insertPoint(std::pair<float, float>& point) {
		int xindex = floor(point.first / cellsize);
		int yindex = floor(point.second / cellsize);
		grid[xindex + yindex * ncells_width] = point;
	}



	std::vector<std::pair<float, float>> poissonDiskSampling(float radius, int k) {
		std::vector<std::pair<float, float>> points, active;

		std::pair<float, float> p0 = std::make_pair(Utility::rand_float(width), Utility::rand_float(height));

		cellsize = std::max(std::floorf(radius / std::sqrtf(N)), 1.f);

		ncells_width = ceil(width / cellsize) + 1;
		ncells_height = ceil(height / cellsize) + 1;

		grid.resize(ncells_width * ncells_height);

		for (int i = 0; i < ncells_width; i++) {
			for (int j = 0; j < ncells_height; j++) {
				grid.at(i + j * ncells_width) = null;
			}
		}

		insertPoint(p0);
		points.push_back(p0);
		active.push_back(p0);

		while (!active.empty()) {
			int random_index = rand() % int(active.size());
			std::pair<float, float> p = active.at(random_index);

			bool found = false;
			for (int tries = 0; tries < k; tries++) {
				float theta = Utility::rand_float(360);
				float new_radius = Utility::rand_float(2 * radius, radius);
				float pnewx = p.first + new_radius * std::cos(radians(theta));
				float pnewy = p.second + new_radius * std::sin(radians(theta));
				std::pair<float, float> pnew = std::make_pair(pnewx, pnewy);
				if (!isValidPoint(pnew, radius)) {
					continue;
				}

				points.push_back(pnew);
				insertPoint(pnew);
				active.push_back(pnew);
				found = true;
				break;
			}
			if (!found)
				active.erase(active.begin() + random_index);
		}

		return points;
	}



public:
	int N = 2;
	int width, height, ncells_width, ncells_height;
	std::vector<std::pair<float, float>> grid;

	float cellsize;
};
