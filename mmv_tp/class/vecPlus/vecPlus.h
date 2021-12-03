#pragma once

#include "mesh.h"
#include "vec.h"
#include <functional>

#define uint  int

template <typename T>
struct vect2 {
	T x, y;
	vect2() :x(), y() {}
	vect2(T v) :x(v), y(v) {}
	vect2(T x, T y) :x(x), y(y) {}
	/*bool operator ==(const vect2& v) {
		if (this == &v)
			return true;
		if (x == v.x && y == v.y)
			return true;
		return false;
	}*/
	bool operator==(const vect2<T>& rhs) {
		if (this == &rhs)
			return true;
		if (x == rhs.x && y == rhs.y)
			return true;
		return false;
	}

};

using Coord2 = vect2<int>;


template <typename T>
struct vect3 {
	T x, y, z;
	vect3() :x(), y(), z() {}
	vect3(T v) :x(v), y(v), z(v) {}
	vect3(T x, T y, T z) :x(x), y(y), z(z) {}
};


namespace std {
	template<typename T>
	T clamp(T v, T min, T max) {
		return (min > v) ? min : (v > max) ? max : v;
	}
};


class gkit_exp {
public:
	static int write_mesh(const Mesh& mesh, const char* filename)
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
};


class m_exp {
public:
	static float gaussian2D(float x, float x_0, float y, float y_0, float A, float sigma_x, float sigma_y);


	static Vector perpendicular_Vector_Triangle(const Vector& a, const Vector& b, const Vector& c);
};

float dot(const vec2& u, const vec2& v);




