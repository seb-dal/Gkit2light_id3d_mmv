#include "MMV_Viewer.h"

#include "../vecPlus/Utility.h"
#include "../aStar/A_Star.h"
#include "../PoissonDisk/PoissonDisk.h"
#include "../vecPlus/gkit_expension.h"


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
				ScalerField w = hf.Wetness();

				h.normelize();
				s.normelize();
				l.normelize();
				a.normelize();
				w.normelize();

#pragma omp parallel for collapse(2)
				for (int y = 0; y < hf.getN().y; y++) {
					for (int x = 0; x < hf.getN().x; x++) {
						texture(x, y) = ColorMaps::Terrain_Map(h.get(x, y), s.get(x, y), l.get(x, y), a.get(x, y), w.get(x, y));
					}
				}


				update_texture();
			}
		)
	);

	functions.push_back(
		function_MMV(
			'o', "poissondisk_test",
			[&](bool ctrl, bool alt, bool shift) -> void {
				if (ctrl) {
#pragma omp parallel for collapse(2)
					for (int y = 0; y < hf.getN().y; y++) {
						for (int x = 0; x < hf.getN().x; x++) {
							texture(x, y) = White();
						}
					}

					PoissonDisk psd(std::make_pair(400, 400));
					auto p = psd.poissonDiskSampling(8, 16);

					for (const auto pp : p) {
						texture(pp.first, pp.second) = Black();
					}

					update_texture();
				}

				if (shift) {
					veget.clear();
					groups_veget.clear();
					veget.create(GL_TRIANGLES);

					PoissonDisk psd(std::make_pair(399, 399));
					auto p = psd.poissonDiskSampling(14, 16);
					for (const auto pp : p) {
						float theta = Utility::rand_float(360);
						Mesh* mm = ms.getRandom_ByName("CommonTree");
						if (mm != nullptr) {

							Transform model =
								Translation(hf.pos(pp.first, pp.second) - Vector(0, 1, 0))
								* RotationY(theta) * Scale(20);

							gkit_exp::add_mesh(veget, mm, model);
						}
					}

					groups_veget = veget.groups();
				}
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

						texture(x, y).r = std::clamp(texture(x, y).r, 0.f, 1.f);
						texture(x, y).g = std::clamp(texture(x, y).g, 0.f, 1.f);
						texture(x, y).b = std::clamp(texture(x, y).b, 0.f, 1.f);

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
					gkit_exp::write_mesh(terrain, name.str().c_str());

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
			'1', "Path_draw",
			[&](bool ctrl, bool alt, bool shift) -> void {
				float
					a = 20,
					b = 70,
					c = 15,
					d = 16
					;

				ScalerField hh = ScalerField(hf);
				ScalerField ss = hf.slopeMap();
				ScalerField ll = hf.laplacien();
				ScalerField aa = hf.AireDrainage();

				hh.normelize();
				ss.normelize();
				ll.normelize();

				aa.normelize();
				aa.minus(0.5);
				aa.abs();

				auto co = Connexite::get_Connexite(Connexite::Type::C8, Connexite::Values::direction, hf.getD().x, hf.getD().y);

				aStar AS(
					co,
					hf.getN().x, hf.getN().y,
					[&](vect2<int> pos) ->float {
						return a * hh.get(pos) + b * ss.get(pos) + c * ll.get(pos) + d * aa.get(pos) + 1;
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

