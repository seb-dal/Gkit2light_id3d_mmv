#include "vecPlus.h"


float m_exp::gaussian2D(float x, float x_0, float y, float y_0, float A, float sigma_x, float sigma_y) {
	float xmx_0 = x - x_0;
	float ymy_0 = y - y_0;
	return
		A * std::exp(
			-(
				(xmx_0 * xmx_0) / (2 * sigma_x * sigma_x)
				+
				(ymy_0 * ymy_0) / (2 * sigma_y * sigma_y)
				)
		);
}




Vector m_exp::perpendicular_Vector_Triangle(const Vector& a, const Vector& b, const Vector& c) {
	return normalize(cross((b - a), (c - a)));
}




float dot(const vec2& u, const vec2& v)
{
	return u.x * v.x + u.y * v.y;
}

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
