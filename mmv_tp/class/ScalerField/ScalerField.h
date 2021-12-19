#pragma once

#include <omp.h>
#include <algorithm>
#include <vector>

#include "image.h"

#include "../BBox/BBox.h"
#include "../vecPlus/vecPlus.h"
#include "../mesh_builder/mesh_builder.h"
#include "../ColorMaps/ColorMaps.h"
#include "../Connnexite/Connexite.h"


struct filter_case {
	int x, y;
	float value;
	filter_case(int x, int y, float value) :x(x), y(y), value(value) {}
};

class ScalerField {


public:
	ScalerField() {};
	ScalerField(BBox b, Coord2 n, vec2 d);
	ScalerField(BBox b, Coord2 n, vec2 d, float init);

	// index of the 2D case at the position x, y
	int getIndex(int x, int y) const;
	// index of the 2D case at the position p
	int getIndex(Coord2 p) const;
	// index of the 2D case at the position p
	int getIndex(std::pair<int, int> p) const;
	// get the coord of the given index
	Coord2 getCoord(int index);

	//Compute a point with coord between [0; 1] if in the domain
	Point pts_domain(const Point& p);

	// tell if the index is inside the list of 2D case
	bool is_valide_Index(int index);


	// tell if the coord 2D are within the list
	bool is_valide_point(int x, int y);
	// tell if the coord 2D are within the list
	bool is_valide_point(Coord2 p);


	// tell if the coord are on the edge  
	bool is_edge(int x, int y);
	// tell if the coord are on the edge  
	bool is_edge(Coord2 p);


	float& get(uint x, uint y);
	float get(uint x, uint y)const;
	float& get(Coord2 p);
	float get(Coord2 p)const;
	float& get(std::pair<int, int> p);
	float get(std::pair<int, int> p)const;
	float& get(int index);
	float get(int index)const;;

	// x and y betwen [0 n]
	float interpolate(const float x, const float y) const;
	float interpolate(const vec2 p) const;

	// x and y betwen [0 1]
	float interp(const float x, const float y) const;
	float interp(const vec2 p) const;

	vec2 Grad_interpolate(const float x, const float y) const;
	vec2 Grad_interp(const float x, const float y) const;
	vec2 Grad(uint i, uint j)const;

	//==============================
	// Math functions

	void sqrt();
	void ln();
	void mult(float v);
	void add(const ScalerField& sf);
	void add(float v);
	void minus(float v);
	void mult(const ScalerField& sf);
	void div(const ScalerField& sf);
	void abs();


	void normelize(float min_o, float max_0, float min, float max);
	void normelize(float min, float max);
	void normelize();


	std::vector<Coord2> voisinage(const std::vector<Coord2>& points, std::vector<Connexite_Coord>& c, bool selfInclude);

	ScalerField laplacien() const;

	ScalerField blur(int half_size = 0);
	ScalerField smooth(int half_size = 0);
	ScalerField smooth(std::vector<Coord2>& applyTo, int half_size = 0);

	// Convolution matrice filter
	ScalerField applyFilter(std::vector<filter_case> mat);
	ScalerField applyFilter(std::vector<filter_case> mat, std::vector<Coord2>& applyTo);


	Mesh to_Mesh(Builder::params p);
	void to_image(Image& img, int tx, int ty);
	void updateMesh(Mesh& m);

public: // GETTER
	Coord2 getN() { return n; }
	BBox getBBox() { return bbox; }
	vec2 getD() { return d; }

protected:
	std::vector<float> vec; // list h
	BBox bbox; // domaine
	Coord2 n; // taille discretisation
	vec2 d, _2d, _dp2;

};

