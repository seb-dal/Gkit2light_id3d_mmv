#pragma once

#include <cmath>
#include <utility>

#include "color.h"


#define M_2TPI 6.28318530717958647693

class ColorMaps {
public:
	static Color HSV_Map(float f);

	static Color Gray_Map(float f);

	static Color Red_Map(float f);

	static Color Blue_Map(float f);

	static Color Green_Map(float f);

	static Color Rainbow_cycle_Map(float f);

	static Color Hot_Map(float f);

	static Color Jet_Map(float f);

	static Color ocean(float f);

	static Color Rainbow_Map(float f);

	static Color Copper_Map(float f);

	static Color Terrain_Map(float f);

	static Color Terrain_Map(float height, float slope, float laplacien, float aireD) {
		Color Gray(0.5, 0.5, 0.5);

		if (height < 0.15) {
			return Blue();
		}
		else if (height < 0.2) {
			if (slope < 0.4)
				return Yellow() * 0.5 + Yellow() * 0.5 * laplacien * aireD * slope;
			else
				return Gray * 0.5 + Gray * aireD * laplacien;
		}
		else if (height < 0.6) {
			if (slope < 0.3)
				return Green() * 0.5 + Gray * aireD * slope * laplacien;
			else
				return Gray * 0.5 + Gray * aireD * laplacien;
		}
		else if (height < 0.8) {
			return Gray * 0.5 + Gray * aireD * laplacien;
		}
		else {
			if (slope < 0.4)
				return White() * 0.5 + Gray * aireD;
			else
				return Gray * 0.5 + Gray * aireD * laplacien;
		}
	}
};
