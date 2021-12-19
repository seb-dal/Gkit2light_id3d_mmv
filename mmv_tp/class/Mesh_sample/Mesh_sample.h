#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "mesh.h"
#include "wavefront.h"



class Mesh_sample {
public:
	Mesh_sample() {};

	/**
	 * Load all the samples mesh.
	 *
	 */
	void load();

	/**
	 * clear and release all samples mesh.
	 *
	 */
	void destroy();

	/**
	 * Get a random mesh in the list with the name of the list of mesh.
	 *
	 * \param name name of the list of mesh
	 * \return
	 */
	Mesh* getRandom_ByName(const char* name);

protected:
	std::vector<std::string> list_name;
	std::unordered_map<std::string, std::vector<Mesh>> map_Mesh;
};

