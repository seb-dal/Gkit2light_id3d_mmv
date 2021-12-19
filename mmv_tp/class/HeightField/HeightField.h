#pragma once

#include <algorithm>
#include <cmath>

#include "../ScalerField/ScalerField.h"
#include "../Image_HF/Image_HF.h"
#include "../Combi_Noise/Combi_Noise.h"
#include "../Connnexite/Connexite.h"

class HeightField :public ScalerField {
public:
	HeightField() {}
	HeightField(Image img, BBox b, Coord2 n, vec2 d);
	HeightField(Noise_combinaison nc, BBox b, Coord2 n, vec2 d);;

	Vector pos(float x, float y);


	Vector Normal(uint i, uint j)const;
	float slope(uint i, uint j);


	ScalerField slopeMap();
	ScalerField AireDrainage();
	ScalerField Wetness();

	// EROSION
	void StreamPowerErosion(ScalerField s, ScalerField A, float k, float dt);
	void HillSlopeErosion(ScalerField lap, float k, float dt);
	void DebrisSlopeErosion(ScalerField s, float k, float dt);


	std::vector<Coord2> CompleteBreach();


	void updateMesh_normal(Mesh& m);

	void blur(int half_size = 0);
	void smooth();
	void smooth(std::vector<Coord2>& applyTo);
	void applyFilter(std::vector<filter_case> mat);
	void applyFilter(std::vector<filter_case> mat, std::vector<Coord2>& applyTo);
};

