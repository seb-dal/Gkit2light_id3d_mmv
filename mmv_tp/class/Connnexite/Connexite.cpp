#include "Connexite.h"

std::vector<Connexite> Connexite::get_Connexite(Type t, Values v, float dx, float dy) {
	std::vector<Connexite> c;
	float dd, ddx, ddy, dddx, dddxx, dddy, dddyy;
	if (int(v) == int(Values::direction)) {
		switch (t) {
		case Type::M3:
			dddx = std::sqrt(9 * dx * dx + dy * dy);
			c.push_back(Connexite(3, 1, dddx));
			c.push_back(Connexite(-3, 1, dddx));
			c.push_back(Connexite(3, -1, dddx));
			c.push_back(Connexite(-3, -1, dddx));

			dddy = std::sqrt(dx * dx + 9 * dy * dy);
			c.push_back(Connexite(1, 3, dddy));
			c.push_back(Connexite(-1, 3, dddy));
			c.push_back(Connexite(1, -3, dddy));
			c.push_back(Connexite(-1, -3, dddy));

			dddxx = std::sqrt(9 * dx * dx + 4 * dy * dy);
			c.push_back(Connexite(3, 2, dddx));
			c.push_back(Connexite(-3, 2, dddx));
			c.push_back(Connexite(3, -2, dddx));
			c.push_back(Connexite(-3, -2, dddx));

			dddyy = std::sqrt(4 * dx * dx + 9 * dy * dy);
			c.push_back(Connexite(2, 3, dddy));
			c.push_back(Connexite(-2, 3, dddy));
			c.push_back(Connexite(2, -3, dddy));
			c.push_back(Connexite(-2, -3, dddy));


		case Type::M2:
			ddx = std::sqrt(4 * dx * dx + dy * dy);
			c.push_back(Connexite(2, 1, ddx));
			c.push_back(Connexite(-2, 1, ddx));
			c.push_back(Connexite(2, -1, ddx));
			c.push_back(Connexite(-2, -1, ddx));

			ddy = std::sqrt(dx * dx + 4 * dy * dy);
			c.push_back(Connexite(1, 2, ddy));
			c.push_back(Connexite(-1, 2, ddy));
			c.push_back(Connexite(1, -2, ddy));
			c.push_back(Connexite(-1, -2, ddy));


		case Type::C8:
			dd = std::sqrt(dx * dx + dy * dy);
			c.push_back(Connexite(1, 1, dd));
			c.push_back(Connexite(-1, 1, dd));
			c.push_back(Connexite(1, -1, dd));
			c.push_back(Connexite(-1, -1, dd));


		case Type::C4:
			c.push_back(Connexite(1, 0, dx));
			c.push_back(Connexite(-1, 0, dx));
			c.push_back(Connexite(0, 1, dy));
			c.push_back(Connexite(0, -1, dy));

		}
	}
	else {

		if (t == Type::C4) {
			c.push_back(Connexite(1, 0, dx));
			c.push_back(Connexite(-1, 0, dx));
			c.push_back(Connexite(0, 1, dy));
			c.push_back(Connexite(0, -1, dy));
		}
		else {
			int s = int(t);
			for (int y = -s; y <= s; y++) {
				for (int x = -s; x <= s; x++) {
					if (x != 0 && y != 0)
						c.push_back(Connexite(x, y, std::sqrt(x * dx * dx + y * dy * dy)));
				}
			}
		}
	}

	return c;
}
