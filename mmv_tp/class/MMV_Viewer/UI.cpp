#include "MMV_Viewer.h"

#include "../PoissonDisk/PoissonDisk.h"
#include "../vecPlus/gkit_expension.h"
#include "../aStar/A_Star.h"

int MMV_Viewer::render_UI() {
	static bool win_erosion = false, win_texture = false, win_manipulation, mb_cullface = false;

	/* Main Winows */ {
		ImGui::Begin("Main Winows");
		/*if (ImGui::BeginMenuBar()) {


		}
		ImGui::EndMenuBar();*/
		if (ImGui::Checkbox("Cull face", &mb_cullface)) {
			if (mb_cullface)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}

		ImGui::Separator();

		ImGui::Checkbox("Windows Erosion", &win_erosion);
		ImGui::Checkbox("Windows Textures", &win_texture);
		ImGui::Checkbox("Windows Manipulation", &win_manipulation);
		ImGui::End();
	}



	/*Erosion Windows*/if (win_erosion) {
		static float
			dt = 0.1,
			spe = 0.65,
			hse = 0.3,
			dse = 0.47;

		ImGui::Begin("Mesh manipulation", &win_erosion);

		ImGui::SliderFloat("delta time", &dt, 0.001f, 10.0f);
		ImGui::SliderFloat("StreamPowerErosion", &spe, 0.0f, 1.0f);
		ImGui::SliderFloat("HillSlopeErosion", &hse, 0.0f, 1.0f);
		ImGui::SliderFloat("DebrisSlopeErosion", &dse, 0.0f, 1.0f);
		if (ImGui::Button("Erosion (e)") || gkit_exp::key_state_then_clear('e')) {
			ScalerField s = hf.slopeMap();
			ScalerField A = hf.AireDrainage();
			ScalerField l = hf.laplacien();

			s.normelize();
			A.normelize();
			A.sqrt();

			l.normelize(-1, 1);

			//printf("start erosion SP \n");
			hf.StreamPowerErosion(s, A, 0.65, dt);
			hf.HillSlopeErosion(l, .3f, dt);
			hf.DebrisSlopeErosion(s, 0.47f, dt);
			//printf("end erosion SP \n");

			update_Mesh();
		}

		ImGui::Separator();

		if (ImGui::Button("Smooth (0)") || gkit_exp::key_state_then_clear('0')) {
			hf.smooth();
			update_Mesh();
		}
		ImGui::SameLine();
		ImGui::Text("  ");
		ImGui::SameLine();
		if (ImGui::Button("Blur (9)") || gkit_exp::key_state_then_clear('9')) {
			hf.blur(2);
			update_Mesh();
		}

		ImGui::Separator();

		bool breaching = gkit_exp::key_state_then_clear('b');

		if (ImGui::Button("Complete Breach") || (breaching && !gkit_exp::ctrl())) {
			hf.CompleteBreach();
			update_Mesh();
		}
		ImGui::SameLine();
		ImGui::Text("  ");
		ImGui::SameLine();
		if (ImGui::Button("Smoothed Complete Breach") || (breaching && gkit_exp::ctrl())) {
			std::vector<Coord2> changed = hf.CompleteBreach();

			std::vector<Coord2> changedEX = hf.voisinage(
				changed,
				Connexite::get_Connexite(Connexite::Type::M3, Connexite::Values::all, 1, 1),
				true);

			hf.smooth(changedEX);

			update_Mesh();
		}

		ImGui::End();
	}/*Erosion Windows*/

	/* Texture Terrain */if (win_texture) {
		ImGui::Begin("Texture terrain", &win_texture);

		if (ImGui::Button("laplacien (l)") || gkit_exp::key_state_then_clear('l')) {
			ScalerField l = hf.laplacien();
			l.normelize();
			l.to_image(texture);

			update_texture();
		}

		if (ImGui::Button("Aire de drainage (a)") || gkit_exp::key_state_then_clear('a')) {
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

		if (ImGui::Button("Slope (s)") || gkit_exp::key_state_then_clear('s')) {
			ScalerField s = hf.slopeMap();
			s.normelize();
			s.to_image(texture);

			update_texture();
		}

		if (ImGui::Button("Wetness (w)") || gkit_exp::key_state_then_clear('w')) {
			ScalerField w = hf.Wetness();
			w.normelize();
			w.to_image(texture);

			update_texture();
		}

		if (ImGui::Button("Terrain (t)") || gkit_exp::key_state_then_clear('t')) {
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

		if (ImGui::Button("Shading (v)") || gkit_exp::key_state_then_clear('v')) {
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

		if (ImGui::Button("Poisson Disk test")) {
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


		ImGui::Separator();
		/* Export */ {
			std::string base_path = "/exported/";
			std::string at = Utility::GetCurrentWorkingDir();
			std::string file;
			file.append(at).append(base_path);
			if (!Utility::folder_exists(file)) {
				Utility::mkdir(file.c_str());
			}

			bool export_item = gkit_exp::key_state_then_clear('x');

			if (ImGui::Button("Export texture (x)") || (export_item && !gkit_exp::ctrl())) {
				std::stringstream name;
				name << "." << base_path << "Export_Texture_" << Utility::getTimeStr() << ".png";

				write_image(texture, name.str().c_str());

				std::cout << "\n" << "EXPORTED CURRENT TEXTURE at: \"" << at << name.str().c_str() << "\"" << std::endl;
			}
			ImGui::SameLine();
			ImGui::Text("  ");
			ImGui::SameLine();
			if (ImGui::Button("Export Mesh (gkit_exp::ctrl-x)") || (export_item && gkit_exp::ctrl())) {
				std::stringstream name;
				name << "." << base_path << "Export_Mesh_" << Utility::getTimeStr() << ".obj";
				gkit_exp::write_mesh(terrain, name.str().c_str());

				std::cout << "\n" << "EXPORTED CURRENT MESH at: \"" << at << name.str().c_str() << "\"" << std::endl;
			}
		}


		ImGui::End();
	} /* Texture Terrain */


	/* Manipulation */ if (win_manipulation) {
		ImGui::Begin("Manipulation", &win_manipulation);

		if (ImGui::TreeNode("Veget")) {
			if (ImGui::Button("Veget")) {
				gen_veget();
			}

			ImGui::TreePop();
		}


		if (ImGui::TreeNode("Path")) {
			static Coord2
				start(10, 10), end(390, 390);

			static float
				coeff_height = 20,
				coeff_slope = 70,
				coeff_laplacien = 15,
				coeff_aire_drainage = 16,
				coeff_wetness = 30;

			if (ImGui::TreeNode("Coord start end")) {
				ImGui::SliderInt("start X", &start.x, 0, hf.getN().x - 1);
				ImGui::SliderInt("start Y", &start.y, 0, hf.getN().y - 1);

				ImGui::SliderInt("end X", &end.x, 0, hf.getN().x - 1);
				ImGui::SliderInt("end Y", &end.y, 0, hf.getN().y - 1);

				ImGui::TreePop();
			}

			ImGui::Separator();

			if (ImGui::TreeNode("Coeff")) {
				ImGui::SliderFloat("cost height", &coeff_height, 0, 200);
				ImGui::SliderFloat("cost slope", &coeff_slope, 0, 200);
				ImGui::SliderFloat("cost laplacien", &coeff_laplacien, 0, 200);
				ImGui::SliderFloat("cost aire drainage", &coeff_aire_drainage, 0, 200);
				ImGui::SliderFloat("cost wetness", &coeff_wetness, 0, 200);

				ImGui::TreePop();
			}

			ImGui::Separator();

			if (ImGui::Button("Path finding")) {
				draw_path(
					start, end,
					coeff_height,
					coeff_slope,
					coeff_laplacien,
					coeff_aire_drainage,
					coeff_wetness
				);
			}

			ImGui::TreePop();
		}


		ImGui::End();
	}

	return 0;
}
