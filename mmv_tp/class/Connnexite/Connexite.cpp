#include "Connexite.h"

std::vector<Connexite_Coord> Connexite_Coord::get_Connexite(Connexite t, Values v, float dx, float dy) {
	std::vector<Connexite_Coord> c;
	float dd, ddx, ddy, dddx, dddxx, dddy, dddyy;
	if (int(v) == int(Values::direction)) {
		switch (t) {
		case Connexite::M3:
			dddx = std::sqrt(9 * dx * dx + dy * dy);
			c.push_back(Connexite_Coord(3, 1, dddx));
			c.push_back(Connexite_Coord(-3, 1, dddx));
			c.push_back(Connexite_Coord(3, -1, dddx));
			c.push_back(Connexite_Coord(-3, -1, dddx));

			dddy = std::sqrt(dx * dx + 9 * dy * dy);
			c.push_back(Connexite_Coord(1, 3, dddy));
			c.push_back(Connexite_Coord(-1, 3, dddy));
			c.push_back(Connexite_Coord(1, -3, dddy));
			c.push_back(Connexite_Coord(-1, -3, dddy));

			dddxx = std::sqrt(9 * dx * dx + 4 * dy * dy);
			c.push_back(Connexite_Coord(3, 2, dddx));
			c.push_back(Connexite_Coord(-3, 2, dddx));
			c.push_back(Connexite_Coord(3, -2, dddx));
			c.push_back(Connexite_Coord(-3, -2, dddx));

			dddyy = std::sqrt(4 * dx * dx + 9 * dy * dy);
			c.push_back(Connexite_Coord(2, 3, dddy));
			c.push_back(Connexite_Coord(-2, 3, dddy));
			c.push_back(Connexite_Coord(2, -3, dddy));
			c.push_back(Connexite_Coord(-2, -3, dddy));


		case Connexite::M2:
			ddx = std::sqrt(4 * dx * dx + dy * dy);
			c.push_back(Connexite_Coord(2, 1, ddx));
			c.push_back(Connexite_Coord(-2, 1, ddx));
			c.push_back(Connexite_Coord(2, -1, ddx));
			c.push_back(Connexite_Coord(-2, -1, ddx));

			ddy = std::sqrt(dx * dx + 4 * dy * dy);
			c.push_back(Connexite_Coord(1, 2, ddy));
			c.push_back(Connexite_Coord(-1, 2, ddy));
			c.push_back(Connexite_Coord(1, -2, ddy));
			c.push_back(Connexite_Coord(-1, -2, ddy));


		case Connexite::C8:
			dd = std::sqrt(dx * dx + dy * dy);
			c.push_back(Connexite_Coord(1, 1, dd));
			c.push_back(Connexite_Coord(-1, 1, dd));
			c.push_back(Connexite_Coord(1, -1, dd));
			c.push_back(Connexite_Coord(-1, -1, dd));


		case Connexite::C4:
			c.push_back(Connexite_Coord(1, 0, dx));
			c.push_back(Connexite_Coord(-1, 0, dx));
			c.push_back(Connexite_Coord(0, 1, dy));
			c.push_back(Connexite_Coord(0, -1, dy));

		}
	}
	else {

		if (t == Connexite::C4) {
			c.push_back(Connexite_Coord(1, 0, dx));
			c.push_back(Connexite_Coord(-1, 0, dx));
			c.push_back(Connexite_Coord(0, 1, dy));
			c.push_back(Connexite_Coord(0, -1, dy));
		}
		else {
			int s = int(t);
			for (int y = -s; y <= s; y++) {
				for (int x = -s; x <= s; x++) {
					if (x != 0 && y != 0)
						c.push_back(Connexite_Coord(x, y, std::sqrt(x * dx * dx + y * dy * dy)));
				}
			}
		}
	}

	return c;
}
