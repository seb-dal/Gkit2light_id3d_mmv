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

#include "../Camera/Camera.h"
#include "../HeightField/HeightField.h"
#include "../Combi_Noise/Combi_Noise.h"
#include "../Mesh_sample/Mesh_sample.h"

class MMV_Viewer : public AppTime
{
	struct function_MMV {
		char key;
		std::string description_function;

		std::function<void(bool, bool, bool)> func;

		function_MMV(char c, std::string str, std::function<void(bool, bool, bool)> func)
			:key(c), description_function(str),
			func(func)
		{}

		bool test()const {
			return key_state(key);
		}

		void operator()()const {
			bool
				ctrl_ = SDL_GetModState() & (KMOD_LCTRL | KMOD_RCTRL),
				alt_ = SDL_GetModState() & (KMOD_LALT | KMOD_RALT),
				shift_ = SDL_GetModState() & (KMOD_LSHIFT | KMOD_RSHIFT);

			if (test()) {
				clear_key_state(key);
				std::cout << description_function << " ";
				func(ctrl_, alt_, shift_);
				std::cout << "\t :DONE" << std::endl;
			}
		}
	};

public:
	MMV_Viewer() : AppTime(1024, 640) {}

	void init_noise();

	void init_functions();

	int init();



	int quit();



	void update_Mesh();

	void update_texture();

	int update(const float time, const float delta);


	void draw_groups(Mesh& m, const Transform& model, const Transform& view, const Transform& projection, std::vector<TriangleGroup>& group) {
		param.model(model).view(view).projection(projection);
		for (int i = 0; i < group.size(); i++) {
			param.draw(group[i], m);
		}
	}


	int render();

protected:
	Mesh_sample ms;

	vect2<int> start = vect2<int>(10, 10), end = vect2<int>(390, 390);

	Noise_combinaison nc;
	Builder::params p = Builder::params(true, true);
	std::vector<function_MMV> functions;

	Camera m_camera;
	HeightField hf;

	Image texture;
	GLuint gl_texture;

	Mesh terrain, veget;

	std::vector<TriangleGroup> groups_veget;

	DrawParam param;
};
