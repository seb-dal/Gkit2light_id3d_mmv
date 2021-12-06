#include "ColorMaps.h"

Color ColorMaps::HSV_Map(float f) {
	float
		R = fabs(f * 6.0f - 3.0f) - 1.0f,
		G = 2.0f - fabs(f * 6.0f - 2.0f),
		B = 2.0f - fabs(f * 6.0f - 4.0f);

	return Color(
		std::max(0.0f, std::min(1.0f, R)),
		std::max(0.0f, std::min(1.0f, G)),
		std::max(0.0f, std::min(1.0f, B))
	);
}

Color ColorMaps::Gray_Map(float f) {
	return Color(f, f, f);
}

Color ColorMaps::Red_Map(float f) {
	return Color(f, 0, 0);
}

Color ColorMaps::Blue_Map(float f) {
	return Color(0, f, 0);
}

Color ColorMaps::Green_Map(float f) {
	return Color(0, 0, f);
}

Color ColorMaps::Rainbow_cycle_Map(float f) {
	float freq = f * M_2TPI;

	return Color(
		(std::cos(freq) + 1) / 2.f,
		(std::cos(freq + (2 / 3.f) * M_PI) + 1) / 2.f,
		(std::cos(freq + (4 / 3.f) * M_PI) + 1) / 2.f
	);
}

Color ColorMaps::Copper_Map(float f) {
	return Color(
		std::min(f * 1.4f, 1.f),
		f / 1.2f,
		f / 2.f
	);
}

Color ColorMaps::Terrain_Map(float f) {
	const float
		p1 = 0.15,
		p2 = 0.25,
		p3 = 0.5,
		p4 = 0.75;

	float x;

	if (f <= p1) {
		x = f / p1;
		return Color(
			0.2f - x * 0.2f,
			0.2f + x * 0.4f,
			0.6f + x * 0.4f
		);
	}
	else if (f <= p2) {
		x = (f - p1) / (p2 - p1);
		return Color(
			0,
			0.6f + x * 0.2f,
			1.f - x * 0.6f
		);
	}
	else if (f <= p3) {
		x = (f - p2) / (p3 - p2);
		return Color(
			x,
			0.8f + x * 0.2f,
			0.4f + x * 0.2f
		);
	}
	else if (f <= p4) {
		x = (f - p3) / (p4 - p3);
		return Color(
			1.f - x * 0.4f,
			1.f - x * 0.6f,
			0.6f - x * 0.3f
		);
	}
	else {
		x = (f - p4) / (1 - p4);
		return Color(
			0.6f + x * 0.4f,
			0.4f + x * 0.6f,
			0.3f + x * 0.7f
		);
	}
}

Color ColorMaps::Terrain_Map(float height, float slope, float laplacien, float aireD, float wetness) {
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

Color ColorMaps::Hot_Map(float f) {
	const float
		p1 = 0.4f,
		p2 = 0.8f;

	if (f < 0.4)
		return Color(f / p1, 0, 0);
	else if (f < 0.8)
		return Color(1, (f - p1) / (p2 - p1), 0);
	else
		return Color(1, 1, (f - p2) / (1.0 - p2));
}

Color ColorMaps::Jet_Map(float f) {
	float
		d = 1.5 * M_PI,
		b = cos((f - (1.f / 4.f)) * d),
		g = cos((f - (2.f / 4.f)) * d),
		r = cos((f - (3.f / 4.f)) * d);

	return Color(r, g, b);
}

Color ColorMaps::ocean(float f) {
	// 68 171
	float
		gx = (68.f / 256.f),
		rx = (171.f / 256.f),
		g = std::max((f - gx) / (1 - gx), 0.f),
		r = std::max((f - rx) / (1 - rx), 0.f);

	return  Color(r, g, f);
}

Color ColorMaps::Rainbow_Map(float f) {
	//103 154 205
	const float
		p1 = 0.4f,
		p2 = 0.6f,
		p3 = 0.8f,
		p4 = 1.05f;

	if (f <= p1) {
		return Color(1, f / p1, 0);
	}
	else if (f <= p2) {
		return Color(1 - (f - p1) / (p2 - p1), 1, 0);
	}
	else if (f <= p3) {
		float v = (f - p2) / (p3 - p2);
		return Color(0, 1 - v, v);
	}
	else {
		return Color((f - p3) / (p4 - p3), 0, 1);
	}
}
