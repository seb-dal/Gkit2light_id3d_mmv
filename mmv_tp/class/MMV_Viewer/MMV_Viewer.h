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

	void load_water(int width, int height);
	void load_texture(int width, int height);
	void load_height_map(const char* path, const Point& domain);


	void init_noise();
	int init();

	int quit();

	void update_Mesh();
	void update_texture();
	int update(const float time, const float delta);

	int render_UI();

	int render();


	//=====================================================================

	void Erode_map(float dt, float spe, float hse, float dse);

	void smooth_map();

	void blur_map();

	void breach_map();

	void smoothed_breaching_map();

	void laplacien_texture();

	void air_drainage_texture();

	void slope_texture();

	void wetness_texture();

	void terrain_texture();

	void shading_texture();

	void poissonDisk_test_texture();

	void Export_init();

	void export_Texture();

	void export_terrain();

protected:
	BBox World_box;

	int
		X_map = 200,
		Y_height_map = 64,
		Z_map = 200;

	int
		X_Water = 200,
		Z_Water = 200;

	int
		X_texture = 2000,
		Y_texture = 2000;

	// Veget variables
	int Factor_scale_Poisson = 2;
	const float
		big_scale = 4,
		small_scale = 0.8,
		big_radius = 8,
		small_radius = 1;


	// water animation variables
	const float
		min_water = 0.13,
		min_water_palier = 0.2,
		water_height_variation = 1.5;

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


	std::string export_path_folder = "/exported/";
	std::string local_folder;
};
