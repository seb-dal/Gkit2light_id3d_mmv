#include "MMV_Viewer.h"
#include "../vecPlus/Utility.h"


void MMV_Viewer::init_functions() {

	functions.push_back(
		function_MMV(
			'l', "laplacien texture",
			[&](bool ctrl, bool alt, bool shift) -> void {
				ScalerField l = hf.laplacien();
				l.normelize();
				l.to_image(texture);

				update_texture();
			}
		)
	);


	functions.push_back(
		function_MMV(
			'a', "Aire de drainage texture",
			[&](bool ctrl, bool alt, bool shift) -> void {
				ScalerField a = hf.AireDrainage();
				a.sqrt();
				a.normelize();
				a.sqrt();
				//a.sqrt();
				//a.sqrt();
				//a.sqrt();

				a.to_image(texture);

				update_texture();
			}
		)
	);


	functions.push_back(
		function_MMV(
			'p', "Slope texture",
			[&](bool ctrl, bool alt, bool shift) -> void {
				ScalerField s = hf.slopeMap();
				s.normelize();
				s.to_image(texture);

				update_texture();
			}
		)
	);


	functions.push_back(
		function_MMV(
			'w', "Wetness texture",
			[&](bool ctrl, bool alt, bool shift) -> void {
				ScalerField w = hf.Wetness();
				w.normelize();
				w.to_image(texture);

				update_texture();
			}
		)
	);



	functions.push_back(
		function_MMV(
			't', "Terrain texture",
			[&](bool ctrl, bool alt, bool shift) -> void {
				ScalerField h = ScalerField(hf);
				ScalerField s = hf.slopeMap();
				ScalerField l = hf.laplacien();
				ScalerField a = hf.AireDrainage();

				h.normelize();
				s.normelize();
				l.normelize();
				a.normelize();

#pragma omp parallel for collapse(2)
				for (int y = 0; y < hf.getN().y; y++) {
					for (int x = 0; x < hf.getN().x; x++) {
						texture(x, y) = ColorMaps::Terrain_Map(h.get(x, y), s.get(x, y), l.get(x, y), a.get(x, y));
					}
				}


				update_texture();
			}
		)
	);


	functions.push_back(
		function_MMV(
			'v', "relief texture",
			[&](bool ctrl, bool alt, bool shift) -> void {
				ScalerField l = hf.laplacien();

				l.normelize();

				vec2 light(-1, 1);

#pragma omp parallel for collapse(2)
				for (int y = 0; y < hf.getN().y; y++) {
					for (int x = 0; x < hf.getN().x; x++) {
						texture(x, y) = Color(0.5) + Color(0.35) * ((dot(hf.Grad(x, y), light) + 1) / 2.f) + Color(0.15) * (-l.get(x, y));

						texture(x, y).a = 1.f;
					}
				}

				update_texture();
			}
		)
	);


	functions.push_back(
		function_MMV(
			'x', "export texture",
			[&](bool ctrl, bool alt, bool shift) -> void {
				std::string base_path = "/exported/";
				std::string at = Utility::GetCurrentWorkingDir();
				std::string file;
				file.append(at).append(base_path);
				if (!Utility::folder_exists(file)) {
					Utility::mkdir(file.c_str());
				}

				if (!ctrl) {
					std::stringstream name;
					name << "." << base_path << "Export_Texture_" << Utility::getTimeStr() << ".png";

					write_image(texture, name.str().c_str());

					std::cout << "\n" << "EXPORTED CURRENT TEXTURE at: \"" << at << name.str().c_str() << "\"" << std::endl;
				}
				else {
					std::stringstream name;
					name << "." << base_path << "Export_Mesh_" << Utility::getTimeStr() << ".obj";
					gkit_exp::write_mesh(m, name.str().c_str());

					std::cout << "\n" << "EXPORTED CURRENT MESH at: \"" << at << name.str().c_str() << "\"" << std::endl;
				}

			}
		)
	);


	functions.push_back(
		function_MMV(
			'b', "Complete Breach",
			[&](bool ctrl, bool alt, bool shift) -> void {
				std::vector<Coord2> changed = hf.CompleteBreach();

				if (ctrl) {
					std::vector<Coord2> changedEX = hf.voisinage(
						changed,
						Connexite::get_Connexite(Connexite::Type::M3, Connexite::Values::all, 1, 1),
						true);

					hf.smooth(changedEX);
				}

				update_Mesh();
			}
		)
	);



	functions.push_back(
		function_MMV(
			'n', "Drainage",
			[&](bool ctrl, bool alt, bool shift) -> void {
				float dt = 2;

				//m.clear();
				//m.release();

				ScalerField s = hf.slopeMap();
				ScalerField A = hf.AireDrainage();
				ScalerField l = hf.laplacien();

				s.normelize();
				A.normelize();
				A.sqrt();
				A.sqrt();

				l.normelize(-1, 1);

				//printf("start erosion SP \n");
				hf.StreamPowerErosion(s, A, 0.65, dt);
				hf.HillSlopeErosion(l, .3f, dt);
				hf.DebrisSlopeErosion(s, 0.47f, dt);
				//printf("end erosion SP \n");

				update_Mesh();
			}
		)
	);


	functions.push_back(
		function_MMV(
			'0', "Smooth height",
			[&](bool ctrl, bool alt, bool shift) -> void {
				hf.smooth();

				update_Mesh();
			}
		)
	);


	functions.push_back(
		function_MMV(
			'9', "Blur height",
			[&](bool ctrl, bool alt, bool shift) -> void {
				hf.blur(2);

				update_Mesh();
			}
		)
	);

}

