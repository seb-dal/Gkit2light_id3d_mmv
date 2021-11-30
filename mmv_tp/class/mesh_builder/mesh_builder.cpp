#include "mesh_builder.h"

Mesh Builder::flat_Mesh(int height, int width, params p) {
	BBox b;
	b.pmax.x = width;
	b.pmax.z = height;
	return flat_Mesh(height, width, b, p);
}

Mesh Builder::flat_Mesh(int size_y, int size_x, BBox b, params p) {
	return  flat_Mesh(size_y, size_x, b, [](int x, int y) { return 0.f; }, p);
}

Mesh Builder::flat_Mesh(int size_y, int size_x, BBox b, std::function<const float(const int x, const int y)> funcZ, params p)
{
	Mesh mesh(GL_TRIANGLES);
	int x, y;
	float s_x = float(size_x), s_y = float(size_y);
	float xx, yy, xxx, yyy;
	for (y = 0; y < size_y; y++) {
		yyy = (float(y) / s_y);
		yy = (float(y) / (s_y - 1));
		for (x = 0; x < size_x; x++) {
			xxx = (float(x) / s_x);
			xx = (float(x) / (s_x - 1));

			if (p.textcoord) {
				mesh.texcoord(vec2(
					xx, yy
				));
			}
			if (p.normal) {
				mesh.normal(vec3(0, 1, 0));
			}

			mesh.vertex(b.pmin + Point(
				xxx * b.pmax.x,
				funcZ(x, y) * b.pmax.y,
				yyy * b.pmax.z
			));
		}
	}

	size_x--;
	size_y--;

	int
		t1, t2, t3, t4, // id vertex
		sy, syp, // temp var
		s_size_x = size_x + 1; // +1 car "y <= size_y" et non "y < size_y"

	for (y = 0; y < size_y; y++) {
		sy = s_size_x * y;
		syp = s_size_x * (y + 1);

		for (x = 0; x < size_x; x++) {
			t1 = x + sy;
			t2 = t1 + 1;
			t3 = x + syp;
			t4 = t3 + 1;

			mesh.triangle(
				t1, t3, t2
			).triangle(
				t2, t3, t4
			);
		}
	}

	return mesh;
}

void Builder::Compute_Height(Mesh& m, int size_y, int size_x, BBox b, std::function<const float(const int x, const int y)> funcZ) {
	int x, y;
	float s_x = float(size_x), s_y = float(size_y);
	float xx, yy, xxx, yyy;
	int i = 0;
	for (y = 0; y < size_y; y++) {
		yyy = (float(y) / s_y);
		yy = (float(y) / (s_y - 1));
		for (x = 0; x < size_x; x++) {
			xxx = (float(x) / s_x);
			xx = (float(x) / (s_x - 1));

			m.vertex(i, b.pmin + Point(
				xxx * b.pmax.x,
				funcZ(x, y) * b.pmax.y,
				yyy * b.pmax.z
			));
			i++;
		}
	}
}



void Builder::Compute_normal(Mesh& m, int size_y, int size_x, std::function<const Vector(const int x, const int y)> funcZ) {
	int x, y;
	float s_x = float(size_x), s_y = float(size_y);
	float xx, yy, xxx, yyy;
	int i = 0;
	for (y = 0; y < size_y; y++) {
		yyy = (float(y) / s_y);
		yy = (float(y) / (s_y - 1));
		for (x = 0; x < size_x; x++) {
			xxx = (float(x) / s_x);
			xx = (float(x) / (s_x - 1));

			m.normal(
				i,
				funcZ(x, y)
			);
			i++;
		}
	}
}

void Builder::Compute_normal(Mesh& m) {
	std::vector<std::pair<Vector, int>> vec(m.vertex_count());

	auto indice = m.indices();
	auto pos = m.positions();

#pragma omp parallel for
	for (int i = 0; i < indice.size(); i += 3) {
		unsigned int
			a = indice[i],
			b = indice[i + 1],
			c = indice[i + 2];

		Vector v = m_exp::perpendicular_Vector_Triangle(pos[a], pos[b], pos[c]);

		vec[a].first = vec[a].first + v;
		vec[b].first = vec[b].first + v;
		vec[c].first = vec[c].first + v;

		vec[a].second++;
		vec[b].second++;
		vec[c].second++;
	}

#pragma omp parallel for
	for (int i = 0; i < vec.size(); i++) {
		m.normal(i, vec[i].first / vec[i].second);
	}
}
