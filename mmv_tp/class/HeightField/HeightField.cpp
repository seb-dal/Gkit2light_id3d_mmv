#include "HeightField.h"
#include <omp.h>



HeightField::HeightField(Image img, BBox b, Coord2 n, vec2 d) :ScalerField(b, n, d) {
#pragma omp parallel for collapse(2)
	for (int y = 0; y < n.y; y++) {
		for (int x = 0; x < n.x; x++) {
			// img(x,y) = [0 1] 
			get(x, y) = img.texture(float(x) / float(n.x), float(y) / float(n.y)).r * (bbox.pmax.y - bbox.pmin.y) - bbox.pmin.y;
		}
	}
}




HeightField::HeightField(Noise_combinaison nc, BBox b, Coord2 n, vec2 d) :ScalerField(b, n, d) {
#pragma omp parallel for collapse(2)
	for (int y = 0; y < n.y; y++) {
		for (int x = 0; x < n.x; x++) {
			// img(x,y) = [0 1] 
			get(x, y) = nc.getHeight(x, y) * (bbox.pmax.y - bbox.pmin.y) - bbox.pmin.y;
		}
	}
}




Vector HeightField::Normal(uint i, uint j) const {
	vec2 g = Grad(i, j);
	return Vector(-g.x, -g.y, 1);
}




float HeightField::slope(uint i, uint j) {
	vec2 g = Grad(i, j);
	return std::sqrtf(g.x * g.x + g.y * g.y);
}




ScalerField HeightField::slopeMap() {
	ScalerField slope(bbox, n, d);

#pragma omp parallel for collapse(2)
	for (int y = 0; y < n.y; y++) {
		for (int x = 0; x < n.x; x++) {
			slope.get(x, y) = this->slope(x, y);
		}
	}
	return slope;
}




ScalerField HeightField::AireDrainage() {
	struct case_a {
		Coord2 pos;
		float z;
		float eau;

		case_a() {}
		case_a(Coord2 pos, float z, float eau) :pos(pos), z(z), eau(eau) {}
		case_a(uint posx, uint posy, float z, float eau) :pos(Coord2(posx, posy)), z(z), eau(eau) {}
	};
	//==========================

	ScalerField AireDrainage(bbox, n, d, 1.f);

	std::vector<case_a> list_case(vec.size());

#pragma omp parallel for collapse(2)
	for (int y = 0; y < n.y; y++) {
		for (int x = 0; x < n.x; x++) {
			list_case[getIndex(x, y)] = case_a(Coord2(x, y), get(x, y), 1);
		}
	}

	std::sort(
		list_case.begin(),
		list_case.end(),
		[](const case_a& lhs, const case_a& rhs) {
			return lhs.z > rhs.z;
		}
	);


	std::vector<Connexite> moveList = Connexite::get_Connexite(Connexite::Type::C4, Connexite::Values::direction, d.x, d.y);
	int size = moveList.size();
	bool direction[8];

	for (const auto& c : list_case) {

		int count = 0;
		for (uint j = 0; j < size; j++) {
			const auto ar = moveList.at(j);
			uint x = c.pos.x + ar.x;
			uint y = c.pos.y + ar.y;

			if (!is_valide_Index(getIndex(x, y)) || c.z < get(x, y)) {
				direction[j] = false;
			}
			else {
				count++;
				direction[j] = true;
			}
		}

		for (uint j = 0; j < size; j++) {
			if (direction[j]) {
				const auto ar = moveList.at(j);
				uint x = c.pos.x + ar.x;
				uint y = c.pos.y + ar.y;

				float ceau = c.eau * (1.f / float(count));
				AireDrainage.get(x, y) += ceau;
				list_case.at(getIndex(x, y)).eau += ceau;
			}
		}

		if (count == 0) {
			AireDrainage.get(c.pos) += c.eau;
		}
	}

	return AireDrainage;
}

ScalerField HeightField::Wetness() {
	ScalerField A = AireDrainage();
	ScalerField s = slopeMap();
	s.add(0.1);
	A.div(s);

	A.ln();
	return A;
}





void HeightField::StreamPowerErosion(ScalerField s, ScalerField A, float k, float dt) {
	A.sqrt();
	s.mult(A);
	s.mult((-k) * dt);
	this->add(s);
}





void HeightField::HillSlopeErosion(ScalerField lap, float k, float dt) {
	lap.mult(k * dt);
	this->add(lap);
}





void HeightField::DebrisSlopeErosion(ScalerField s, float k, float dt) {
	s.mult(-k * dt);
	this->add(s);
}






void HeightField::updateMesh_normal(Mesh& m) {
	Builder::Compute_normal(m, n.x, n.y,
		[&](const int x, const int y) {
			Vector n = Normal(x, y);
			return normalize(Vector(n.x, n.z, n.y));
		}
	);
}





void HeightField::blur(int half_size) {
	std::vector<filter_case> blur_filter;
	for (int y = -half_size; y <= half_size; y++) {
		for (int x = -half_size; x <= half_size; x++) {
			blur_filter.push_back(
				filter_case(x, y,
					1. / float((2 * half_size + 1) * (2 * half_size + 1))
				)
			);
		}
	}
	applyFilter(blur_filter);
}


static bool init_vec_smooth = false;
static std::vector<filter_case> blur_smooth;
static void init_blur_smooth() {
	blur_smooth.push_back(filter_case(-1, 1, 1 / 16.f));
	blur_smooth.push_back(filter_case(1, 1, 1 / 16.f));
	blur_smooth.push_back(filter_case(-1, -1, 1 / 16.f));
	blur_smooth.push_back(filter_case(1, -1, 1 / 16.f));

	blur_smooth.push_back(filter_case(-1, 0, 2 / 16.f));
	blur_smooth.push_back(filter_case(1, 0, 2 / 16.f));
	blur_smooth.push_back(filter_case(0, 1, 2 / 16.f));
	blur_smooth.push_back(filter_case(0, -1, 2 / 16.f));

	blur_smooth.push_back(filter_case(0, 0, 4 / 16.f));
	init_vec_smooth = true;
}


void HeightField::smooth() {
	if (!init_vec_smooth)
		init_blur_smooth();

	applyFilter(blur_smooth);
}

void HeightField::smooth(std::vector<Coord2>& applyTo) {
	if (!init_vec_smooth)
		init_blur_smooth();

	applyFilter(blur_smooth, applyTo);
}





void HeightField::applyFilter(std::vector<filter_case> mat) {
	ScalerField sf(bbox, n, d);
#pragma omp parallel for collapse(2)
	for (int y = 0; y < n.y; y++) {
		for (int x = 0; x < n.x; x++) {
			for (filter_case& f : mat) {
				sf.get(x, y) += get(
					std::clamp(f.x + x, 0, int(n.x - 1)),
					std::clamp(f.y + y, 0, int(n.y - 1))
				) * f.value;
			}
		}
	}

#pragma omp parallel for collapse(2)
	for (int y = 0; y < n.y; y++) {
		for (int x = 0; x < n.x; x++) {
			get(x, y) = sf.get(x, y);
		}
	}
}

void HeightField::applyFilter(std::vector<filter_case> mat, std::vector<Coord2>& applyTo) {
	ScalerField sf(bbox, n, d);

#pragma omp parallel for
	for (int i = 0; i < applyTo.size(); i++) {
		const Coord2 p = applyTo.at(i);
		for (filter_case& f : mat) {
			sf.get(p) += get(
				std::clamp(f.x + p.x, 0, int(n.x - 1)),
				std::clamp(f.y + p.y, 0, int(n.y - 1))
			) * f.value;
		}
	}
#pragma omp parallel for
	for (int i = 0; i < applyTo.size(); i++) {
		const Coord2 p = applyTo.at(i);
		get(p) = sf.get(p);
	}
}
