#pragma once

#include <cmath>
#include <vector>

class Connexite_Coord {
private:
	Connexite_Coord(int x, int y, double dist) :x(x), y(y), dist(dist) {}

public:
	enum class Connexite {
		C4, C8, M2, M3
	};
	enum class Values {
		all, direction
	};

	static std::vector<Connexite_Coord> get_Connexite(Connexite t, Values v, float dx, float dy);


public:
	int x;
	int y;
	double dist;
};

