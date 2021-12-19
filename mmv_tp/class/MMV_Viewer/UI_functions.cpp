#include "MMV_Viewer.h"

#include "../PoissonDisk/PoissonDisk.h"
#include "../vecPlus/Utility.h"
#include "../vecPlus/gkit_expension.h"




//=====================================================================

void MMV_Viewer::update_water_mesh(const float time) {
	const auto& pos = water.positions();

	float time_speed = time * 0.01;

	float time_x = cos(time_speed * 0.01) * sin(0.368 + time_speed * 0.008) * 100;
	float time_z = sin(time_speed * 0.01) * cos(1.2358 + time_speed * 0.015) * 100;


#pragma omp parallel for
	for (int i = 0; i < water.vertex_count(); i++) {
		Point p = pos[i];
		Point pp = hf.pts_domain(p);
		if (hf.interp(pp.x, pp.z) < World_box.pmax.y * min_water_palier) {
			p.y = World_box.pmax.y * min_water + Water_noise.getHeight(p.x + time_x, p.z + time_z) * water_height_variation;
			water.vertex(i, p);
		}
	}
}





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
		Connexite_Coord::get_Connexite(Connexite_Coord::Connexite::M3, Connexite_Coord::Values::all, 1, 1),
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



void MMV_Viewer::poissonDisk_double_test_texture() {
#pragma omp parallel for collapse(2)
	for (int y = 0; y < Y_texture; y++) {
		for (int x = 0; x < X_texture; x++) {
			texture(x, y) = White();
		}
	}

	PoissonDisk
		psd(std::make_pair(X_texture, Y_texture), 16, 16),
		psd2(std::make_pair(X_texture, Y_texture), 4, 16);

	auto p = psd.poissonDiskSampling();
	for (const auto& pp : p) {
		psd2.add_point(pp);
	}

	auto p2 = psd2.poissonDiskSampling();
	for (const auto& pp : p2) {
		texture(pp.first, pp.second) = Green();
	}

	for (const auto& pp : p) {
		texture(pp.first, pp.second) = Red();
	}


	update_texture();
}





void MMV_Viewer::Export_init() {
	local_folder = Utility::GetCurrentWorkingDir();
	std::string file;
	file.append(local_folder).append(export_path_folder);
	if (!Utility::folder_exists(file)) {
		Utility::mkdir(file.c_str());
	}
}





void MMV_Viewer::export_Texture() {
	std::stringstream name;
	name << "." << export_path_folder << "Export_Texture_" << Utility::getTimeStr() << ".png";

	write_image(texture, name.str().c_str());

	std::cout << "\n" << "EXPORTED CURRENT TEXTURE at: \"" << local_folder << name.str().c_str() << "\"" << std::endl;
}





void MMV_Viewer::export_terrain() {
	std::stringstream name;
	name << "." << export_path_folder << "Export_Mesh_" << Utility::getTimeStr() << ".obj";
	gkit_exp::write_mesh(terrain, name.str().c_str());

	std::cout << "\n" << "EXPORTED CURRENT MESH at: \"" << local_folder << name.str().c_str() << "\"" << std::endl;
}

