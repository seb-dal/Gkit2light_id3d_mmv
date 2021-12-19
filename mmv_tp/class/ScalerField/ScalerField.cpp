#include "ScalerField.h"
#include "../vecPlus/Unique.h"

ScalerField::ScalerField(BBox b, Coord2 n, vec2 d)
	:bbox(b), n(n), d(d),
	_2d(vec2(d.x * 2, d.y * 2)),
	_dp2(vec2(d.x* d.x, d.y* d.y)),
	vec(n.x* n.y)
{}

ScalerField::ScalerField(BBox b, Coord2 n, vec2 d, float init)
	:bbox(b), n(n), d(d),
	_2d(vec2(d.x * 2, d.y * 2)),
	_dp2(vec2(d.x* d.x, d.y* d.y)),
	vec(n.x* n.y, init)
{}




int ScalerField::getIndex(int x, int y) const { return x + n.x * y; }
int ScalerField::getIndex(Coord2 p) const { return p.x + n.x * p.y; }
int ScalerField::getIndex(std::pair<int, int> p) const { return p.first + n.x * p.second; }



Coord2 ScalerField::getCoord(int index) { return Coord2(index % n.x, index / n.x); }

Point ScalerField::pts_domain(const Point& p) {
	Point pp = p;
	pp.x = (pp.x - bbox.pmin.x) / bbox.pmax.x;
	pp.y = (pp.y - bbox.pmin.y) / bbox.pmax.y;
	pp.z = (pp.z - bbox.pmin.z) / bbox.pmax.z;
	return pp;
}



bool ScalerField::is_valide_Index(int index) { return index >= 0 && index < vec.size(); }


// tell if the coord 2D are within the list

bool ScalerField::is_valide_point(int x, int y) { return x >= 0 && x < n.x&& y >= 0 && y < n.y; }

bool ScalerField::is_valide_point(Coord2 p) { return is_valide_point(p.x, p.y); }



bool ScalerField::is_edge(int x, int y) { return x == 0 || x == (n.x - 1) || y == 0 || y == (n.y - 1); }

bool ScalerField::is_edge(Coord2 p) { return is_edge(p.x, p.y); }



float& ScalerField::get(uint x, uint y) { return vec[getIndex(x, y)]; }
float ScalerField::get(uint x, uint y) const { return vec[getIndex(x, y)]; }
float& ScalerField::get(Coord2 p) { return vec[getIndex(p)]; }
float ScalerField::get(Coord2 p) const { return vec[getIndex(p)]; }
float& ScalerField::get(std::pair<int, int> p) { return vec[getIndex(p)]; }
float ScalerField::get(std::pair<int, int> p) const { return vec[getIndex(p)]; }
float& ScalerField::get(int index) { return vec[index]; }
float ScalerField::get(int index) const { return vec[index]; }




float ScalerField::interpolate(const float x, const float y) const
{
	float xx = std::clamp(x, 0.f, float(n.x - 2));
	float yy = std::clamp(y, 0.f, float(n.y - 2));

	// interpolation bilineaire 
	float u = xx - std::floor(xx);
	float v = yy - std::floor(yy);
	int ix = xx;
	int iy = yy;
	return get(ix, iy) * ((1 - u) * (1 - v))
		+ get(ix + 1, iy) * (u * (1 - v))
		+ get(ix, iy + 1) * ((1 - u) * v)
		+ get(ix + 1, iy + 1) * (u * v);
}


vec2 ScalerField::Grad_interpolate(const float x, const float y) const
{
	float xx = std::clamp(x, 0.f, float(n.x - 2));
	float yy = std::clamp(y, 0.f, float(n.y - 2));

	// interpolation bilineaire 
	float u = xx - std::floor(xx);
	float v = yy - std::floor(yy);
	int ix = xx;
	int iy = yy;

	vec2 a = Grad(ix, iy);
	a(0) *= ((1 - u) * (1 - v));
	a(1) *= ((1 - u) * (1 - v));

	vec2 b = Grad(ix + 1, iy);
	b(0) *= (u * (1 - v));
	b(1) *= (u * (1 - v));

	vec2 c = Grad(ix, iy + 1);
	c(0) *= ((1 - u) * v);
	c(1) *= ((1 - u) * v);

	vec2 d = Grad(ix + 1, iy + 1);
	d(0) *= (u * v);
	d(1) *= (u * v);


	return vec2(a.x + b.x + c.x + d.x, a.y + b.y + c.y + d.y);
}

vec2 ScalerField::Grad_interp(const float x, const float y) const
{
	return Grad_interpolate(x * n.x, y * n.y);
}


float ScalerField::interpolate(const vec2 p) const {
	return interpolate(p.x, p.y);
}

float ScalerField::interp(const float x, const float y) const
{
	return interpolate(x * n.x, y * n.y);
}

float ScalerField::interp(const vec2 p) const
{
	return interp(p.x, p.y);
}





vec2 ScalerField::Grad(uint x, uint y) const
{
	float
		xm = std::min(x + 1, n.x - 1),
		xM = std::max(x - 1, 0),
		ym = std::min(y + 1, n.y - 1),
		yM = std::max(y - 1, 0);

	return vec2(
		(get(xM, y) - get(xm, y)) / _2d.x,
		(get(x, yM) - get(x, ym)) / _2d.y
	);
}





void ScalerField::sqrt() {
#pragma omp parallel for 
	for (int i = 0; i < vec.size(); i++) {
		vec[i] = std::sqrtf(vec[i]);
	}
}

void ScalerField::ln() {
#pragma omp parallel for 
	for (int i = 0; i < vec.size(); i++) {
		vec[i] = std::logf(vec[i]);
	}
}





void ScalerField::mult(float v) {
#pragma omp parallel for 
	for (int i = 0; i < vec.size(); i++) {
		vec[i] *= v;
	}
}





void ScalerField::add(const ScalerField& sf) {
#pragma omp parallel for 
	for (int i = 0; i < vec.size(); i++) {
		vec[i] += sf.vec[i];
	}
}

void ScalerField::add(float v) {
#pragma omp parallel for 
	for (int i = 0; i < vec.size(); i++) {
		vec[i] += v;
	}
}

void ScalerField::minus(float v) {
#pragma omp parallel for 
	for (int i = 0; i < vec.size(); i++) {
		vec[i] -= v;
	}
}





void ScalerField::mult(const ScalerField& sf) {
#pragma omp parallel for 
	for (int i = 0; i < vec.size(); i++) {
		vec[i] *= sf.vec[i];
	}
}

void ScalerField::div(const ScalerField& sf) {
#pragma omp parallel for 
	for (int i = 0; i < vec.size(); i++) {
		vec[i] /= sf.vec[i];
	}
}

void ScalerField::abs() {
#pragma omp parallel for 
	for (int i = 0; i < vec.size(); i++) {
		vec[i] = std::abs(vec[i]);
	}
}





ScalerField ScalerField::laplacien() const {
	ScalerField laplacien(bbox, n, d);

	//#pragma omp parallel for collapse(2)
	for (int y = 0; y < n.y; y++) {
		for (int x = 0; x < n.x; x++) {
			laplacien.get(x, y) =
				(
					get(std::clamp(x - 1, 0, int(n.x - 1)), y)
					+ get(std::clamp(x + 1, 0, int(n.x - 1)), y)
					- 2 * get(x, y)
					) / (_dp2.x)
				+
				(
					get(x, std::clamp(y - 1, 0, int(n.y - 1)))
					+ get(x, std::clamp(y + 1, 0, int(n.y - 1)))
					- 2 * get(x, y)
					) / (_dp2.y);
		}
	}


	return laplacien;
}





void ScalerField::normelize(float min_o, float max_0, float min, float max) {
	float div = max_0 - min_o;
#pragma omp parallel for
	for (int i = 0; i < vec.size(); i++) {
		vec[i] = ((vec[i] - min_o) / (div)) * (max - min) + min;
	}
}

void ScalerField::normelize(float min, float max) {
	const auto minmax = std::minmax_element(vec.begin(), vec.end());

	normelize(*minmax.first, *minmax.second, min, max);
}


void ScalerField::normelize() {
	normelize(0, 1);
}






std::vector<Coord2> ScalerField::voisinage(const std::vector<Coord2>& points, std::vector<Connexite>& c, bool selfInclude) {
	std::vector<Coord2> voisin;

	Unique<Coord2> u;
	u.set_Hash_function(
		[](const Coord2& e)-> std::string {
			std::string r;
			r.append(std::to_string(e.x)).append(" ").append(std::to_string(e.y));
			return r;
		}
	);

	for (const auto cc : c) {
		for (const auto p : points) {
			const int x = p.x + cc.x;
			const int y = p.y + cc.y;
			Coord2 pp(x, y);
			if (is_valide_point(pp)) {
				u.add(pp);
			}
		}
	}

	if (selfInclude) {
		for (const auto p : points) {
			if (is_valide_point(p)) {
				u.add(p);
			}
		}
	}
	voisin.resize(u.size());
	int i = 0;
	for (auto it = u.begin(); it != u.end(); i++, it++)
		voisin[i] = (*it).second;
	//voisin.assign(u.begin(), u.end());
	return voisin;
}





Mesh ScalerField::to_Mesh(Builder::params p) {
	Mesh mesh = Builder::flat_Mesh(
		n.x, n.y,
		bbox,
		[&](const int x, const int y) { return get(x, y) / (bbox.pmax.y - bbox.pmin.y); },
		p
	);

	return mesh;
}





ScalerField ScalerField::blur(int half_size) {
	std::vector<filter_case> blur_filter;
	for (int y = -half_size; y <= half_size; y++) {
		for (int x = -half_size; x <= half_size; x++) {
			blur_filter.push_back(filter_case(x, y, 1. / ((2 * half_size + 1) * (2 * half_size + 1))));
		}
	}
	return applyFilter(blur_filter);
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


ScalerField ScalerField::smooth(int half_size) {
	if (!init_vec_smooth)
		init_blur_smooth();

	return applyFilter(blur_smooth);
}

ScalerField ScalerField::smooth(std::vector<Coord2>& applyTo, int half_size) {
	if (!init_vec_smooth)
		init_blur_smooth();

	return applyFilter(blur_smooth, applyTo);
}




ScalerField ScalerField::applyFilter(std::vector<filter_case> mat) {
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

	return sf;
}

ScalerField ScalerField::applyFilter(std::vector<filter_case> mat, std::vector<Coord2>& applyTo) {
	ScalerField sf(bbox, n, d);

#pragma omp parallel for
	for (int i = 0; i < applyTo.size(); i++) {
		const Coord2 p = applyTo.at(i);
		for (filter_case& f : mat) {
			sf.get(p.x, p.y) += get(
				std::clamp(f.x + p.x, 0, int(n.x - 1)),
				std::clamp(f.y + p.y, 0, int(n.y - 1))
			) * f.value;
		}
	}

	return sf;
}





void ScalerField::to_image(Image& img, int tx, int ty) {
#pragma omp parallel for collapse(2)
	for (int y = 0; y < ty; y++) {
		for (int x = 0; x < tx; x++) {
			img(x, y) = ColorMaps::Rainbow_Map(interp(x / float(tx), y / float(ty)));
		}
	}
}





void ScalerField::updateMesh(Mesh& m) {
	Builder::Compute_Height(m, n.x, n.y, bbox, [&](const int x, const int y) { return get(x, y) / (bbox.pmax.y - bbox.pmin.y); });
}
