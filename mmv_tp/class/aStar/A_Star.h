#pragma once

#include <list>
#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <stdexcept>
#include <queue>
#include <unordered_map>
#include <string>

#include "../Connnexite/Connexite.h"
#include "../vecPlus/vecPlus.h"
#include "../vecPlus/Utility.h"

// http://www.rosettacode.org/wiki/A*_search_algorithm#C.2B.2B



using point = vect2<int>;





class map_obstacle {
public:
	map_obstacle(unsigned int size_x, unsigned int size_y) :
		size_x(size_x), size_y(size_y),
		map(size_x* size_y, true)
	{}

	bool operator() (int x, int y) { return map[x + y * size_x]; }

	std::vector<bool> map;
	int size_x, size_y;
};






class node {
public:
	bool operator == (const node& o) const { return pos == o.pos; }
	bool operator == (const point& o) const { return pos == o; }
	bool operator < (const node& o) const { return dist + cost < o.dist + o.cost; }

	point pos, parent;
	float dist, cost;
};


//									  <<dist, cost>, id_node>
using Elem_astar = std::pair<std::pair<float, float>, size_t>;

struct comparator {
	// Compare elevation, then x, then y
	bool operator()(const Elem_astar& a, const Elem_astar& b) {
		return a.first.second > b.first.second;
		if (a.first.second > b.first.second) {
			return true;
		}
		else {
			return a.first.first > b.first.first;
		}
	}
};



class aStar {
	std::string to_name(const point& p) {
		std::string s;
		s.reserve(32);
		s += Utility::fast_int2str(p.x);
		s += " ";
		s += Utility::fast_int2str(p.y);
		return s;
	}
public:
	aStar(std::vector<Connexite> connexite, unsigned int size_x, unsigned int size_y, std::function<float(point)> cost_func) :
		/*m(size_x, size_y),*/
		neighbours(connexite),
		cost_func(cost_func),
		MAX(size_x, size_y)
	{}


	int calcDist(point& p) {
		// need a better heuristic
		int x = end.x - p.x, y = end.y - p.y;
		return(x * x + y * y);
	}


	bool isValid(point& p) {
		return (p.x > -1 && p.y > -1 && p.x < MAX.x&& p.y < MAX.y);
	}


	bool existPoint(point& p, float cost, float dist, point& parent) {

		std::string key = to_name(p);

		if (map_pos_open.find(key) != map_pos_open.end()) {
			node& n = nodes.at(map_pos_open[key]);
			if (n.cost + n.dist < cost + dist) {
				return true;
			}
			else {
				n.cost = cost;
				n.dist = dist;
				n.parent = parent;
				return true;
			}
		}

		if (map_pos_close.find(key) != map_pos_close.end()) {
			node& n = nodes.at(map_pos_close[key]);
			if (n.cost + n.dist < cost + dist) {
				return true;
			}
			else {
				n.cost = cost;
				n.dist = dist;
				n.parent = parent;
				return true;
			}
		}


		return false;
	}



	bool fillOpen(node& parent) {
		float stepCost;
		float nc, dist;
		point neighbour;

		for (Connexite& c : neighbours)
		{
			neighbour = parent.pos + point(c.x, c.y);


			if (isValid(neighbour)/* && m(neighbour.x, neighbour.y)*/) {

				stepCost = c.dist * cost_func(neighbour);
				nc = stepCost + parent.cost;
				dist = calcDist(neighbour);
				if (!existPoint(neighbour, nc, dist, parent.pos)) {
					node m;
					m.cost = nc;
					m.dist = dist;
					m.pos = neighbour;
					m.parent = parent.pos;

					add_open_elem(m);
				}
			}

			if (neighbour == end) return true;
		}


		return false;
	}



	bool search(point& start, point& end) {
		node n;
		this->end = end; this->start = start;

		n.cost = 0;
		n.pos = start;
		n.parent = point(-1, -1);
		n.dist = calcDist(start);

		add_open_elem(n);
		//open.push_back(n);
		while (!sorted_open.empty()) {
			//open.sort(); // 28.57 %

			const auto id = sorted_open.top();
			node nn = nodes.at(id.second);
			//open.erase(open.begin());
			sorted_open.pop();

			closed.push_back(id.second);
			map_pos_close.emplace(to_name(nn.pos), id.second);

			if (fillOpen(nn)) // 66.67 %
				return true;
		}
		return false;
	}


	float path(std::list<point>& path) {
		node n = nodes[closed.back()];
		float cost = n.cost;

		path.push_back(n.pos);
		while (map_pos_close.find(to_name(n.parent)) != map_pos_close.end()) {
			n = nodes[map_pos_close[to_name(n.parent)]];
			path.push_back(n.pos);
		}

		return cost;
	}





	//map_obstacle m;
	point MAX;
	point end, start;
	std::vector<Connexite> neighbours;
	std::function<float(point)> cost_func;

	//std::list<node> open;
	std::list<size_t> closed;


	void add_open_elem(node& n) {
		nodes.push_back(n);
		size_t ns = nodes.size() - 1;
		map_pos_open.emplace(to_name(n.pos), ns);
		sorted_open.emplace(
			std::make_pair(
				std::make_pair(n.dist, n.cost),
				ns
			)
		);
	}

	//open :  -map:pos    -sortedList:cout
	std::unordered_map<std::string, size_t> map_pos_open;
	std::unordered_map<std::string, size_t> map_pos_close;

	std::priority_queue<Elem_astar, std::vector<Elem_astar>, comparator> sorted_open;

	// listes nodes
	std::vector<node> nodes;
};

