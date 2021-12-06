#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "mesh.h"
#include "wavefront.h"



class Mesh_sample {
public:
	Mesh_sample() {};
	void load();

	void destroy();

	Mesh* getRandom_ByName(const char* name);

	std::vector<std::string> list_name;
	std::unordered_map<std::string, std::vector<Mesh>> map_Mesh;
};

