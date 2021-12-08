#include "Mesh_sample.h"

void Mesh_sample::load() {
	std::string folder_base = "./id3d_mmv/data/Ultimate Nature Pack/OBJ/";

	std::pair<std::string, int> folder_name[] = {
		{ "CommonTree", 5 },
		{ "PalmTree", 5 },
		{ "Rock", 7 },
		{ "Rock_Moss", 7 },
		{ "Rock_Snow", 7 },
		{ "PineTree", 5},
		{ "PineTree_Snow", 5}
	};

	for (auto c : folder_name) {
		std::vector<Mesh> list_mesh_type;
		for (int i = 1; i <= c.second; i++) {
			list_mesh_type.push_back(
				read_indexed_mesh(std::string(folder_base + c.first + "/" + c.first + "_" + std::to_string(i) + ".obj").c_str())
			);
		}
		list_name.push_back(c.first);
		map_Mesh[c.first] = list_mesh_type;
	}

}

void Mesh_sample::destroy() {
	for (std::string name : list_name) {
		for (Mesh m : map_Mesh[name]) {
			m.release();
			m.clear();
		}
	}
}

Mesh* Mesh_sample::getRandom_ByName(const char* name) {

	if (map_Mesh.find(name) == map_Mesh.end()) {
		return nullptr;
	}
	else {
		auto& list = map_Mesh[name];
		return &(list.at(rand() % int(list.size())));
	}

}
