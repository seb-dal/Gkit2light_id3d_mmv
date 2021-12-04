#include "HeightField.h"
#include <omp.h>
#include <queue>


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





std::vector<Coord2> HeightField::CompleteBreach() {
	using Element = std::pair<double, Coord2>;
	struct comp {

		// Compare elevation, then x, then y
		bool operator()(const Element& a, const Element& b)
		{
			const auto& lhs = a.second;
			const auto& rhs = b.second;

			if (a.first > b.first) return true;
			else if ((a.first == b.first) && (lhs.x > rhs.x || (lhs.x == rhs.x && lhs.y > rhs.y)))
				return true;
			else return false;
		}
	};
	using ZPriorityQueue = std::priority_queue< Element, std::vector<Element>, comp >;

	enum CellType {
		UNVISITED,
		VISITED,
		EDGE
	};
	const int NO_BACK_LINK = -1;

	std::vector<Connexite> moveList = Connexite::get_Connexite(Connexite::Type::C4, Connexite::Values::direction, d.x, d.y);
	//==========================================================================
	std::vector<Coord2> modified;

	ScalerField
		visited(bbox, n, d, CellType::UNVISITED),
		pits(bbox, n, d, 0),
		backlinks(bbox, n, d, NO_BACK_LINK);

	ZPriorityQueue pq;

	constexpr float float_min = 1e-6; //std::numeric_limits<float>::lowest();
	constexpr float float_max = std::numeric_limits<float>::max();

	int total_pits = 0, x, y;

	//#pragma omp parallel for collapse(2)
	for (x = 0; x < n.x; x++) {
		for (y = 0; y < n.y; y++) {
			if (is_edge(x, y)) {

				pq.emplace(get(x, y), Coord2(x, y));
				visited.get(x, y) = CellType::EDGE;

				continue;
			}

			float lowest_neighbour = float_max;
			for (Connexite ar : moveList) {
				const int px = x + ar.x;
				const int py = y + ar.y;

				lowest_neighbour = std::min(get(px, py), lowest_neighbour);
			}

			if (get(x, y) < lowest_neighbour) {
				get(x, y) = lowest_neighbour - 2.0 * float_min;
			}

			if (get(x, y) <= lowest_neighbour) {
				pits.get(x, y) = 1;
				total_pits++;
			}

		}
	}



	while (!pq.empty()) {
		const auto c = pq.top();
		pq.pop();

		const float elevation = c.first;
		const Coord2 p = c.second;


		if (pits.get(p.x, p.y)) {
			int cc = getIndex(p);

			const float pathdepth = -2.0 * float_min;
			double target_height = get(cc);



			while (cc != NO_BACK_LINK && get(cc) >= target_height) {
				get(cc) = target_height;
				modified.push_back(getCoord(cc));
				cc = backlinks.get(cc);
				target_height = target_height - 2.0 * float_min;
			}


			--total_pits;
			if (total_pits <= 0)
				break;
		}


		for (auto& m : moveList)
		{
			const int px = p.x + m.x;
			const int py = p.y + m.y;

			if (!is_valide_Index(getIndex(px, py)))
				continue;

			if (visited.get(px, py) != CellType::UNVISITED)
				continue;

			const float my_e = get(px, py);

			//The neighbour is unvisited. Add it to the queue
			pq.emplace(my_e, Coord2(px, py));
			//if (fill_depressions)
			//flood_array.emplace_back(VertexIndex(px, py));
			visited.get(px, py) = CellType::VISITED;
			backlinks.get(px, py) = getIndex(p);
		}

		/* if (mode != COMPLETE_BREACHING && fill_depressions)
		{
		for (const auto f : flood_array)
		{
		int parent = backlinks.at(f);
		if (at(f) <= at(parent))
		{
		field[f] = nextafter(at(parent), std::numeric_limits<double>::max());
		}
		}
		}*/

	}
	return modified;
}





void HeightField::updateMesh_normal(Mesh& m) {
	Builder::Compute_normal(m, n.y, n.x,
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
