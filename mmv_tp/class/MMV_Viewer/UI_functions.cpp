#include "MMV_Viewer.h"

#include "../PoissonDisk/PoissonDisk.h"




void MMV_Viewer::Erode_map(float dt, float spe, float hse, float dse) {
	ScalerField s = hf.slopeMap();
	ScalerField A = hf.AireDrainage();
	ScalerField l = hf.laplacien();

	s.normelize();
	A.normelize();
	A.sqrt();

	l.normelize(-1, 1);

	//printf("start erosion SP \n");
	hf.StreamPowerErosion(s, A, spe, dt);
	hf.HillSlopeErosion(l, hse, dt);
	hf.DebrisSlopeErosion(s, dse, dt);
	//printf("end erosion SP \n");

	update_Mesh();
}





void MMV_Viewer::smooth_map() {
	hf.smooth();
	update_Mesh();
}





void MMV_Viewer::blur_map() {
	hf.blur(2);
	update_Mesh();
}





void MMV_Viewer::breach_map() {
	hf.CompleteBreach();
	update_Mesh();
}





void MMV_Viewer::smoothed_breaching_map() {
	std::vector<Coord2> changed = hf.CompleteBreach();

	std::vector<Coord2> changedEX = hf.voisinage(
		changed,
		Connexite::get_Connexite(Connexite::Type::M3, Connexite::Values::all, 1, 1),
		true);

	hf.smooth(changedEX);

	update_Mesh();
}





void MMV_Viewer::laplacien_texture() {
	ScalerField l = hf.laplacien();
	l.normelize();
	l.to_image(texture, X_texture, Y_texture);

	update_texture();
}





void MMV_Viewer::air_drainage_texture() {
	ScalerField a = hf.AireDrainage();
	a.sqrt();
	a.normelize();
	a.sqrt();

	a.to_image(texture, X_texture, Y_texture);

	update_texture();
}





void MMV_Viewer::slope_texture() {
	ScalerField s = hf.slopeMap();
	s.normelize();
	s.to_image(texture, X_texture, Y_texture);

	update_texture();
}





void MMV_Viewer::wetness_texture() {
	ScalerField w = hf.Wetness();
	w.normelize();
	w.to_image(texture, X_texture, Y_texture);

	update_texture();
}





void MMV_Viewer::terrain_texture() {
	ScalerField h = ScalerField(hf);
	ScalerField s = hf.slopeMap();
	ScalerField l = hf.laplacien();
	ScalerField a = hf.AireDrainage();
	ScalerField w = hf.Wetness();

	h.normelize(World_box.pmin.y, World_box.pmax.y, 0, 1);
	s.normelize();
	l.normelize();
	a.normelize();
	w.normelize();

#pragma omp parallel for collapse(2)
	for (int y = 0; y < Y_texture; y++) {
		for (int x = 0; x < X_texture; x++) {
			float xx = x / float(X_texture);
			float yy = y / float(Y_texture);

			texture(x, y) = ColorMaps::Terrain_Map(
				h.interp(xx, yy),
				s.interp(xx, yy),
				l.interp(xx, yy),
				a.interp(xx, yy),
				w.interp(xx, yy)
			);
		}
	}

	update_texture();
}





void MMV_Viewer::shading_texture() {
	ScalerField l = hf.laplacien();

	l.normelize();

	vec2 light(-1, 1);

#pragma omp parallel for collapse(2)
	for (int y = 0; y < Y_texture; y++) {
		for (int x = 0; x < X_texture; x++) {
			float xx = x / float(X_texture);
			float yy = y / float(Y_texture);

			texture(x, y) = Color(0.5) + Color(0.35) * ((dot(hf.Grad_interp(xx, yy), light) + 1) / 2.f) + Color(0.15) * (-l.interp(xx, yy));

			texture(x, y).r = std::clamp(texture(x, y).r, 0.f, 1.f);
			texture(x, y).g = std::clamp(texture(x, y).g, 0.f, 1.f);
			texture(x, y).b = std::clamp(texture(x, y).b, 0.f, 1.f);

			texture(x, y).a = 1.f;
		}
	}

	update_texture();
}





void MMV_Viewer::poissonDisk_test_texture() {
#pragma omp parallel for collapse(2)
	for (int y = 0; y < Y_texture; y++) {
		for (int x = 0; x < X_texture; x++) {
			texture(x, y) = White();
		}
	}

	PoissonDisk psd(std::make_pair(X_texture, Y_texture), 8, 16);
	auto p = psd.poissonDiskSampling();

	for (const auto pp : p) {
		texture(pp.first, pp.second) = Black();
	}

	update_texture();
}

