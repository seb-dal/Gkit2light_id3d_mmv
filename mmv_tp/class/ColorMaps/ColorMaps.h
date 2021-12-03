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

	static Color Terrain_Map(float height, float slope, float laplacien, float aireD);
};
