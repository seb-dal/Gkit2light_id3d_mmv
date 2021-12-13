#pragma once

#include <omp.h>
#include <sstream>
#include <functional>
#include <vector>


#include "mat.h"
#include "mesh.h"
#include "wavefront.h"

#include "orbiter.h"
#include "program.h"
#include "uniforms.h"
#include "draw.h"
#include "app_time.h"
#include "texture.h"
#include "image_io.h"

#include "../../lib/gkit2light_imgui/app_imgui.h"

#include "../Camera/Camera.h"
#include "../HeightField/HeightField.h"
#include "../Combi_Noise/Combi_Noise.h"
#include "../Mesh_sample/Mesh_sample.h"


class MMV_Viewer : public App_imgui
{
	void gen_veget();

	void draw_path(
		Coord2& start, Coord2& end,
		float coeff_height,
		float coeff_slope,
		float coeff_laplacien,
		float coeff_aire_drainage,
		float coeff_wetness
	);

public:
	MMV_Viewer() : App_imgui(1020, 540) {}

	void init_noise();
	int init();

	int quit();

	void update_Mesh();
	void update_texture();
	int update(const float time, const float delta);

	int render_UI();

	int render();

protected:
	BBox map_box;
	int height_map = 400,
		width_map = 400;

	const float big_scale = 4, small_scale = 0.8, big_radius = 8, small_radius = 1;
	const float min_water = 0.15, min_water_palier = 0.19, height_water = 2.5, water_level = 1.2;

	Mesh_sample ms;

	Noise_combinaison nc;
	Builder::params
		p = Builder::params(true, true),
		pw = Builder::params(false, false, Color(0.314, 0.635, 1.0));

	Camera m_camera;
	HeightField hf;


	Image texture;
	GLuint gl_texture;

	Noise_combinaison Water_noise;

	bool render_water = false, render_veget = true;
	Mesh terrain, veget, water;
	std::vector<TriangleGroup> groups_veget;

};
