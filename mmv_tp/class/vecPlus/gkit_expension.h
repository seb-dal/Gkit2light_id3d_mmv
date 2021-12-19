#pragma once

#include "mesh.h"
#include "vec.h"
#include "mat.h"
#include "draw.h"

class gkit_exp {
public:
	// write a material-less mesh into a file.
	static int write_mesh(const Mesh& mesh, const char* filename);

	// add all the material of a mesh into another if not exist 
	static std::vector<int> init_materials(Mesh& m, Mesh* mptr);

	// add a mesh into another
	static void add_mesh(Mesh& m, Mesh* m2, Transform t);

	// draw material color group of mesh
	static void draw_groups(Mesh& m, const Transform& model, const Transform& view, const Transform& projection, std::vector<TriangleGroup>& group);


	static bool key_state_then_clear(SDL_Keycode key);

	static bool ctrl();
	static bool alt();
	static bool shift();
};
