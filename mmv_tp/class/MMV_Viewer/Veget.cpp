#include "MMV_Viewer.h"

#include "../PoissonDisk/PoissonDisk.h"
#include "../vecPlus/gkit_expension.h"

#include "../vecPlus/Utility.h"

static std::pair<float, float> scale_down(std::pair<float, float>& pst, float factor) {
	return std::make_pair(
		pst.first / factor,
		pst.second / factor
	);
}

void MMV_Viewer::gen_veget() {
	veget.clear();
	groups_veget.clear();
	veget.create(GL_TRIANGLES);

	ScalerField hh = ScalerField(hf);
	ScalerField ss = hf.slopeMap();
	ScalerField ll = hf.laplacien();
	ScalerField aa = hf.AireDrainage();
	ScalerField ww = hf.Wetness();

	hh.normelize(World_box.pmin.y, World_box.pmax.y, 0, 1);
	ss.normelize();
	ll.normelize();
	ww.normelize();

	aa.normelize();
	aa.minus(0.5);
	aa.abs();
	aa.mult(2);

	auto size_map = std::make_pair((X_map - 1) * Factor_scale_Poisson, (Z_map - 1) * Factor_scale_Poisson);

	PoissonDisk
		psd(size_map, big_radius, 16),
		psd_2(size_map, small_radius, 16);

	auto p = psd.poissonDiskSampling();
	for (auto& ppp : p) {
		auto pp = scale_down(ppp, Factor_scale_Poisson);

		psd_2.add_point(pp);

		float height = hh.get(pp);
		float slope = ss.get(pp);
		float wetness = ww.get(pp);

		float random = Utility::rand_float();
		float theta = Utility::rand_float(360);
		Transform model =
			Translation(hf.pos(pp.first, pp.second) - Vector(0, 0.5 * big_scale, 0))
			* RotationY(theta) * Scale(big_scale) * Scale(1 + Utility::rand_float() / 2.f, 1 + Utility::rand_float(), 1 + Utility::rand_float() / 2.f);


		if (height < 0.15) {
			// nothing
		}
		else if (height < 0.2) {
			if (slope < 0.4) {
				if (random > 0.75) {
					gkit_exp::add_mesh(veget, ms.getRandom_ByName("PalmTree"), model);
				}
			}
			else {
				gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock"), model);
			}
		}
		else if (height < 0.6) {
			if (slope < 0.3) {
				if (wetness > 0.7) {
					// nothing
				}
				else if (wetness > 0.5) {
					if (random < 0.75) {
						if (random > 0.5) {
							gkit_exp::add_mesh(veget, ms.getRandom_ByName("BushBerries"), model);
						}
						else {
							gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock_Moss"), model);
						}
					}
				}
				else {
					if (random > 0.05) {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("CommonTree"), model);
					}
					else {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("BirchTree"), model);
					}
				}
			}
			else {
				if (random > 0.5) {
					gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock_Moss"), model);
				}
				else {
					gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock"), model);
				}
			}
		}
		else if (height < 0.8) {
			if (slope < 0.4) {
				gkit_exp::add_mesh(veget, ms.getRandom_ByName("PineTree"), model);
			}
			else {
				gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock"), model);
			}
		}
		else {
			if (slope < 0.3) {
				if (random < 0.5) {
					if (random > 0.1) {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("PineTree_Snow"), model);
					}
					else {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("Bush_Snow"), model);
					}
				}
			}
			else {
				gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock_Snow"), model);
			}
		}
	}






	auto p2 = psd_2.poissonDiskSampling();
	for (auto& ppp : p2) {
		auto pp = scale_down(ppp, Factor_scale_Poisson);

		float height = hh.get(pp);
		float slope = ss.get(pp);
		float wetness = ww.get(pp);

		float random = Utility::rand_float();
		float theta = Utility::rand_float(360);
		Transform model =
			Translation(hf.pos(pp.first, pp.second) - Vector(0, 0.1 * small_scale, 0))
			* RotationY(theta) * Scale(small_scale) * Scale(1 + Utility::rand_float(), 1 + Utility::rand_float(), 1 + Utility::rand_float());

		if (height < 0.15) {
			// nothing
		}
		else if (height < 0.2) {
			//if (slope < 0.2) {
			//}
		}
		else if (height < 0.6) {
			if (slope < 0.4) {
				if (wetness > 0.7) {
					if (random > 0.8) {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("Wheat"), model);
					}
					else {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("Grass"), model);
					}
				}
				else if (wetness > 0.5) {
					if (random < 0.5) {
						if (random > 0.45) {
							gkit_exp::add_mesh(veget, ms.getRandom_ByName("Bush"), model);
						}
						else if (random > 0.4) {
							gkit_exp::add_mesh(veget, ms.getRandom_ByName("BushBerries"), model);
						}
						else if (random > 0.3) {
							gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock_Moss"), model);
						}
					}
				}
				else {
					if (random > 0.9) {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("Flowers"), model);
					}
					else if (random > 0.7) {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("Plant"), model);
					}
				}
			}
			else {
				if (random < 0.2) {
					if (random > 0.1) {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock_Moss"), model);
					}
					else {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock"), model);
					}
				}
			}
		}
		else if (height < 0.8) {
			if (random > 0.9) {
				if (slope < 0.3) {
					gkit_exp::add_mesh(veget, ms.getRandom_ByName("PineTree"), model);
				}
				else {
					gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock"), model);
				}
			}
		}
		else {
			if (slope < 0.25) {
				if (random < 0.05) {
					if (random > 0.02) {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("PineTree_Snow"), model);
					}
					else if (random > 0.015) {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("Bush_Snow"), model);
					}
				}
			}
			else {
				if (random > 0.5) {
					gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock_Snow"), model);
				}
			}
		}
	}



	groups_veget = veget.groups();
}

