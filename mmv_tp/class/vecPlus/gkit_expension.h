#pragma once

#include "mesh.h"
#include "vec.h"
#include <mat.h>

class gkit_exp {
public:
	static int write_mesh(const Mesh& mesh, const char* filename);

	static std::vector<int> init_materials(Mesh& m, Mesh* mptr);

	static void add_mesh(Mesh& m, Mesh* m2, Transform t);
};
