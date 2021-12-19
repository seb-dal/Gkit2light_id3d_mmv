#include "MMV_Viewer.h"

#include "../PoissonDisk/PoissonDisk.h"
#include "../vecPlus/gkit_expension.h"
#include "../aStar/A_Star.h"


static bool
win_erosion = false,
win_texture = false,
win_manipulation = false,
mb_cullface = false,
main_open = false,
open_file_windows = false;


#define PATH_SIZE 600
static char
path_buf[PATH_SIZE] = "./id3d_mmv/mmv_tp/data/a.jpg";



#define MIN_SIZE_TEXTURE 50
#define MIN_SIZE_MAP 50

static std::string
waring_load = "";

static std::string extension[] = {
	".png", ".jpg"
};

int MMV_Viewer::render_UI() {
	static int
		X_map_ = X_map,
		Y_map_ = Y_height_map,
		Z_map_ = Z_map;
	static int
		X_texture_ = X_texture,
		Y_texture_ = Y_texture;
	static int
		X_Water_ = X_Water,
		Z_Water_ = Z_Water;



	if (gkit_exp::key_state_then_clear(SDLK_F1))
		main_open = !main_open;

	if (gkit_exp::key_state_then_clear(SDLK_F2))
		win_erosion = !win_erosion;

	if (gkit_exp::key_state_then_clear(SDLK_F3))
		win_texture = !win_texture;

	if (gkit_exp::key_state_then_clear(SDLK_F4))
		win_manipulation = !win_manipulation;



	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::MenuItem("Open Height field", "Ctrl+O")) {
			open_file_windows = !open_file_windows;
		}

		if (ImGui::MenuItem("Export terrain Height field", "Ctrl+x")) {
			export_terrain();
		}

		ImGui::EndMainMenuBar();
	}


	/* Terrain loading winows */if (open_file_windows) {
		ImGui::Begin("Open Height field", &open_file_windows);

		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), waring_load.c_str());

		ImGui::InputText("Path", path_buf, PATH_SIZE);


		ImGui::InputInt("Domain X map", &X_map_);
		if (X_map_ < MIN_SIZE_MAP) { X_map_ = MIN_SIZE_MAP; }
		ImGui::InputInt("Domain Y map", &Y_map_);
		if (Y_map_ < MIN_SIZE_MAP) { Y_map_ = MIN_SIZE_MAP; }
		ImGui::InputInt("Domain Z map", &Z_map_);
		if (Z_map_ < MIN_SIZE_MAP) { Z_map_ = MIN_SIZE_MAP; }

		if (ImGui::Button("Load")) {
			if (Utility::str_iendwith(std::string(path_buf), extension)) {
				if (Utility::file_exist(path_buf)) {

					load_height_map(path_buf, Point(X_map_, Y_map_, Z_map_));


				}
				else {
					waring_load = "Le chemin vers le fichier n'est pas valide";
				}
			}
			else {
				waring_load = "L'extension du fichier n'est pas valide, seul .png et .jpg sont utilisable";
			}
		}
		ImGui::End();
	}



	/* Main Winows */ {
		ImGui::SetNextWindowCollapsed(main_open);

		main_open = !ImGui::Begin("Main Winows (F1)");

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


		if (ImGui::Checkbox("Cull face", &mb_cullface)) {
			if (mb_cullface)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}

		if (ImGui::Button("Switch camera mode (c)")) {
			m_camera.switch_view_mode();
		}

		ImGui::Checkbox("render water", &render_water);

		ImGui::Separator();

		ImGui::Checkbox("Windows Erosion (F2)", &win_erosion);
		ImGui::Checkbox("Windows Textures (F3)", &win_texture);
		ImGui::Checkbox("Windows Manipulation (F4)", &win_manipulation);
		ImGui::End();
	}



	/*Erosion Windows*/if (win_erosion) {
		static float
			dt = 0.1,
			spe = 0.65,
			hse = 0.3,
			dse = 0.47;

		ImGui::Begin("Mesh manipulation (F2)", &win_erosion);

		ImGui::SliderFloat("delta time", &dt, 0.001f, 10.0f);
		ImGui::SliderFloat("StreamPowerErosion", &spe, 0.0f, 1.0f);
		ImGui::SliderFloat("HillSlopeErosion", &hse, 0.0f, 1.0f);
		ImGui::SliderFloat("DebrisSlopeErosion", &dse, 0.0f, 1.0f);
		if (ImGui::Button("Erosion (e)") || gkit_exp::key_state_then_clear('e')) {
			Erode_map(dt, spe, hse, dse);
		}

		ImGui::Separator();

		if (ImGui::Button("Smooth (0)") || gkit_exp::key_state_then_clear('0')) {
			smooth_map();
		}
		ImGui::SameLine();
		ImGui::Text("  ");
		ImGui::SameLine();
		if (ImGui::Button("Blur (9)") || gkit_exp::key_state_then_clear('9')) {
			blur_map();
		}

		ImGui::Separator();

		bool breaching = gkit_exp::key_state_then_clear('b');

		if (ImGui::Button("Complete Breach (b)") || (breaching && !gkit_exp::ctrl())) {
			breach_map();
		}
		ImGui::SameLine();
		ImGui::Text("  ");
		ImGui::SameLine();
		if (ImGui::Button("Smoothed Complete Breach (ctrl-b)") || (breaching && gkit_exp::ctrl())) {
			smoothed_breaching_map();
		}

		ImGui::End();
	}/*Erosion Windows*/





	/* Texture Terrain */if (win_texture) {
		ImGui::Begin("Texture terrain (F3)", &win_texture);

		ImGui::InputInt("Domain X map", &X_texture_);
		if (X_texture_ < MIN_SIZE_MAP) { X_texture_ = MIN_SIZE_MAP; }
		ImGui::InputInt("Domain Y map", &Y_texture_);
		if (Y_texture_ < MIN_SIZE_MAP) { Y_texture_ = MIN_SIZE_MAP; }

		if (ImGui::Button("Change size Texture")) {
			load_texture(X_texture_, Y_texture_);
		}

		ImGui::Separator();

		if (ImGui::Button("laplacien (l)") || gkit_exp::key_state_then_clear('l')) {
			laplacien_texture();
		}

		if (ImGui::Button("Aire de drainage (a)") || gkit_exp::key_state_then_clear('a')) {
			air_drainage_texture();
		}

		if (ImGui::Button("Slope (s)") || gkit_exp::key_state_then_clear('s')) {
			slope_texture();
		}

		if (ImGui::Button("Wetness (w)") || gkit_exp::key_state_then_clear('w')) {
			wetness_texture();
		}

		if (ImGui::Button("Terrain (t)") || gkit_exp::key_state_then_clear('t')) {
			terrain_texture();
		}

		if (ImGui::Button("Shading (v)") || gkit_exp::key_state_then_clear('v')) {
			shading_texture();
		}

		if (ImGui::Button("Poisson Disk test")) {
			poissonDisk_test_texture();
		}


		ImGui::Separator();
		/* Export */ {


			bool export_item = gkit_exp::key_state_then_clear('x');

			if (ImGui::Button("Export texture (x)") || (export_item && !gkit_exp::ctrl())) {
				export_Texture();
			}
		}


		ImGui::End();
	} /* Texture Terrain */


	/* Manipulation */ if (win_manipulation) {
		ImGui::Begin("Manipulation (F4)", &win_manipulation);

		if (ImGui::TreeNode("Veget")) {
			if (ImGui::Button("Veget (v)") || gkit_exp::key_state_then_clear('v')) {
				gen_veget();
			}

			static bool hide;
			if (ImGui::Checkbox("Hide", &hide)) {
				render_veget = !hide;
			}

			if (ImGui::Button("Clear")) {
				groups_veget.clear();
				veget.clear();
			}

			ImGui::TreePop();
		}


		if (ImGui::TreeNode("Water")) {
			ImGui::InputInt("Size X water", &X_Water_);
			ImGui::InputInt("Size Z water", &Z_Water_);

			if (ImGui::Button("Update water")) {
				load_water(X_Water_, Z_Water_);
			}

			ImGui::TreePop();
		}


		if (ImGui::TreeNode("Path")) {
			static Coord2
				start(10, 10), end(X_texture - 10, Y_texture - 10);

			static float
				coeff_height = 0,
				coeff_slope = 70,
				coeff_laplacien = 15,
				coeff_aire_drainage = 16,
				coeff_wetness = 30;

			if (ImGui::TreeNode("Coord start end")) {
				ImGui::SliderInt("start X", &start.x, 0, X_texture - 1);
				ImGui::SliderInt("start Y", &start.y, 0, Y_texture - 1);

				ImGui::SliderInt("end X", &end.x, 0, X_texture - 1);
				ImGui::SliderInt("end Y", &end.y, 0, Y_texture - 1);

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
