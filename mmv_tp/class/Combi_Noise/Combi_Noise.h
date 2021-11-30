#pragma once

#include <vector>

#include "../../lib/FastNoise/FastNoise.h"

class Noise_combinaison {
public:
	Noise_combinaison() {}

	Noise_combinaison& add(FastNoiseLite& noise) {
		listes_noise.push_back(noise);
		return *this;
	}

	float getHeight(float x, float z) {
		float sum = 0;
		for (FastNoiseLite fnl : listes_noise) {
			sum = fnl.GetNoise(x, z);
		}
		return sum;
	}

private:
	std::vector<FastNoiseLite> listes_noise;
};

