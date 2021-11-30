#pragma once

#include <cmath>
#include <vector>

class Connexite {
private:
	Connexite(int x, int y, double dist) :x(x), y(y), dist(dist) {}

public:
	enum class Type {
		C4, C8, M2, M3
	};
	enum class Values {
		all, direction
	};

	static std::vector<Connexite> get_Connexite(Type t, Values v, float dx, float dy);


public:
	int x;
	int y;
	double dist;
};

