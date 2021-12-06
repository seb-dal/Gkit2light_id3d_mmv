#include "gkit_expension.h"

int gkit_exp::write_mesh(const Mesh& mesh, const char* filename)
{
	if (mesh == Mesh::error())
		return -1;

	if (mesh.primitives() != GL_TRIANGLES)
		return -1;
	if (mesh.positions().size() == 0)
		return -1;
	if (filename == NULL)
		return -1;

	FILE* out = fopen(filename, "wt");
	if (out == NULL)
		return -1;

	printf("writing mesh '%s'...\n", filename);


	const std::vector<vec3>& positions = mesh.positions();
	for (unsigned i = 0; i < positions.size(); i++)
		fprintf(out, "v %f %f %f\n", positions[i].x, positions[i].y, positions[i].z);
	fprintf(out, "\n");



	const std::vector<vec2>& texcoords = mesh.texcoords();
	bool has_texcoords = (texcoords.size() == positions.size());
	if (mesh.has_texcoord()) {
		for (unsigned i = 0; i < texcoords.size(); i++)
			fprintf(out, "vt %f %f\n", texcoords[i].x, texcoords[i].y);
		fprintf(out, "\n");
	}



	const std::vector<vec3>& normals = mesh.normals();
	bool has_normals = (normals.size() == positions.size());
	if (mesh.has_normal()) {
		for (unsigned i = 0; i < normals.size(); i++)
			fprintf(out, "vn %f %f %f\n", normals[i].x, normals[i].y, normals[i].z);
		fprintf(out, "\n");
	}



	int material_id = -1;
	const std::vector<unsigned>& materials = mesh.material_indices();

	const std::vector<unsigned>& indices = mesh.indices();
	bool has_indices = (indices.size() > 0);
	unsigned n = has_indices ? indices.size() : positions.size();
	for (unsigned i = 0; i + 2 < n; i += 3)
	{

		if (!materials.empty() && material_id != int(materials[i / 3]))
		{
			material_id = int(materials[i / 3]);
			if (material_id != -1)
				fprintf(out, "usemtl %s\n", mesh.materials().name(material_id));
		}

		fprintf(out, "f");
		for (unsigned k = 0; k < 3; k++)
		{
			unsigned id = has_indices ? indices[i + k] + 1 : i + k + 1;
			fprintf(out, " %u", id);
			if (has_texcoords && has_normals)
				fprintf(out, "/%u/%u", id, id);
			else if (has_texcoords)
				fprintf(out, "/%u", id);
			else if (has_normals)
				fprintf(out, "//%u", id);
		}
		fprintf(out, "\n");
	}


	fclose(out);
	return 0;
}






std::vector<int> gkit_exp::init_materials(Mesh& m, Mesh* mptr) {
	std::vector<int> id_mat_in_m;
	int find;
	Materials& mat = m.materials();
	const Materials& mat2 = mptr->materials();

	for (int i = 0; i < mat2.count(); i++) {
		find = mat.find(mat2.names[i].c_str());
		if (find == -1) {
			id_mat_in_m.push_back(
				mat.insert(mat2.materials[i], mat2.names[i].c_str())
			);
		}
		else {
			id_mat_in_m.push_back(find);
		}
	}

	return id_mat_in_m;
}







void gkit_exp::add_mesh(Mesh& m, Mesh* m2, Transform t) {
	auto mat_m = init_materials(m, m2);

	int start = m.vertex_count();

	//auto& nor = m2->normals();
	auto& pts = m2->positions();
	for (int i = 0; i < pts.size(); i++) {
		//m.normal(nor[i]);
		m.vertex(t(Point(pts[i])));
	}

	auto& ids = m2->indices();
	auto& m_pts = m2->material_indices();
	if (!ids.empty()) {
		for (int i = 0; i * 3 < ids.size(); i++) {
			m.material(mat_m[m_pts[std::min(i, int(m_pts.size() - 1))]]);
			m.triangle(
				start + ids[i * 3],
				start + ids[i * 3 + 1],
				start + ids[i * 3 + 2]
			);
		}
	}
}
