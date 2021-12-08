#include "HeightField.h"

#include <omp.h>
#include <queue>



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

