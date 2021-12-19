#include "MMV_Viewer.h"

#include "../aStar/A_Star.h"

void MMV_Viewer::draw_path(
	Coord2& start, Coord2& end,
	float coeff_height,
	float coeff_slope,
	float coeff_laplacien,
	float coeff_aire_drainage,
	float coeff_wetness
) {
	ScalerField hh = ScalerField(hf);
	ScalerField ss = hf.slopeMap();
	ScalerField ll = hf.laplacien();
	ScalerField aa = hf.AireDrainage();
	ScalerField ww = hf.Wetness();

	hh.normelize(World_box.pmin.y, World_box.pmax.y, 0, 1);
	ss.normelize(0, 90, 0, 1);
	ll.normelize();
	ww.normelize();

	aa.normelize();
	aa.minus(0.5);
	aa.abs();
	aa.mult(2);

	auto co = Connexite_Coord::get_Connexite(
		Connexite_Coord::Connexite::C8, Connexite_Coord::Values::direction,
		hf.getD().x, hf.getD().y
	);

	aStar AS(
		co,
		X_texture, Y_texture,
		[&](vect2<int> pos) ->float {
			float xx = pos.x / float(X_texture);
			float yy = pos.y / float(Y_texture);

			return
				coeff_height * hh.interp(xx, yy)
				+ coeff_slope * ss.interp(xx, yy)
				+ coeff_laplacien * ll.interp(xx, yy)
				+ coeff_aire_drainage * aa.interp(xx, yy)
				+ coeff_wetness * ww.interp(xx, yy)
				+ 1;
		}
	);
	AS.search(start, end);

	for (const auto& p : AS.map_pos_open) {
		const auto& n = AS.nodes[p.second];
		const int x = n.pos.x;
		const int y = n.pos.y;

		texture(x, y) = Blue();
	}

	for (const auto& p : AS.map_pos_close) {
		const auto& n = AS.nodes[p.second];
		const int x = n.pos.x;
		const int y = n.pos.y;

		texture(x, y) = Green();
	}

	std::list<vect2<int>> path;
	AS.path(path);

	for (const auto& p : path) {
		const int x = p.x;
		const int y = p.y;

		texture(x, y) = Red();
	}

	update_texture();
}
