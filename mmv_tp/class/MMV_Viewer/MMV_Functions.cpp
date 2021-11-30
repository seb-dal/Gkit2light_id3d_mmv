#include "MMV_Viewer.h"
#include "../vecPlus/Utility.h"


void MMV_Viewer::init_functions() {

	functions.push_back(
		function_MMV(
			'l', "laplacien texture",
			[&]() -> void {
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
			[&]() -> void {
				ScalerField a = hf.AireDrainage();
				//a.sqrt();
				a.normelize();
				a.sqrt();
				a.sqrt();
				a.sqrt();
				a.sqrt();

				a.to_image(texture);

				update_texture();
			}
		)
	);


	functions.push_back(
		function_MMV(
			'p', "Slope texture",
			[&]() -> void {
				ScalerField s = hf.slopeMap();
				s.normelize();
				s.to_image(texture);

				update_texture();
			}
		)
	);


	functions.push_back(
		function_MMV(
			't', "Terrain texture",
			[&]() -> void {
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
			[&]() -> void {
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
			[&]() -> void {
				std::stringstream name;
				name << "Export_Texture_" << Utility::getTimeStr() << ".png";

				write_image(texture, name.str().c_str());

				std::cout << "\n" << "EXPORTED CURRENT TEXTURE: \"" << name.str().c_str() << "\"" << std::endl;
			}
		)
	);


	functions.push_back(
		function_MMV(
			'b', "Complete Breach",
			[&]() -> void {
				hf.CompleteBreach();

				update_Mesh();
			}
		)
	);

	functions.push_back(
		function_MMV(
			'b', "Smoothed Complete Breach",
			[&]() -> void {
				std::vector<Coord2> changed = hf.CompleteBreach();
				std::vector<Coord2> changedEX = hf.voisinage(
					changed,
					Connexite::get_Connexite(Connexite::Type::M3, Connexite::Values::all, 1, 1),
					true);

				hf.smooth(changedEX);
				update_Mesh();
			},
			true, false, false
				)
	);


	functions.push_back(
		function_MMV(
			'n', "drainage",
			[&]() -> void {
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
				hf.StreamPowerErosion(s, A, 0.95, dt);
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
			[&]() -> void {
				hf.smooth();

				update_Mesh();
			}
		)
	);


	functions.push_back(
		function_MMV(
			'9', "Blur height",
			[&]() -> void {
				hf.blur(2);

				update_Mesh();
			}
		)
	);

}

