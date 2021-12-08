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
	MMV_Viewer() : App_imgui(1024, 640) {}

	void init_noise();


	int init();



	int quit();



	void update_Mesh();

	void update_texture();

	int update(const float time, const float delta);



	int render_UI();

	int render();

protected:
	Mesh_sample ms;

	Noise_combinaison nc;
	Builder::params p = Builder::params(true, true);

	Camera m_camera;
	HeightField hf;


	Image texture;
	GLuint gl_texture;

	Mesh terrain, veget;
	std::vector<TriangleGroup> groups_veget;

};
