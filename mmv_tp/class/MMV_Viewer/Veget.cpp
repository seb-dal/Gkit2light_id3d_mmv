#include "MMV_Viewer.h"

#include "../PoissonDisk/PoissonDisk.h"
#include "../vecPlus/gkit_expension.h"


void MMV_Viewer::gen_veget() {
	veget.clear();
	groups_veget.clear();
	veget.create(GL_TRIANGLES);

	ScalerField hh = ScalerField(hf);
	ScalerField ss = hf.slopeMap();
	ScalerField ll = hf.laplacien();
	ScalerField aa = hf.AireDrainage();
	ScalerField ww = hf.Wetness();

	hh.normelize();
	ss.normelize();
	ll.normelize();
	ww.normelize();

	aa.normelize();
	aa.minus(0.5);
	aa.abs();
	aa.mult(2);



	PoissonDisk psd(std::make_pair(399, 399));
	auto p = psd.poissonDiskSampling(10, 16);
	for (const auto pp : p) {
		float height = hh.get(pp);
		float slope = ss.get(pp);
		float wetness = ww.get(pp);

		float random = Utility::rand_float();
		float theta = Utility::rand_float(360);
		Transform model =
			Translation(hf.pos(pp.first, pp.second) - Vector(0, 1, 0))
			* RotationY(theta) * Scale(20);


		if (height < 0.15) {
			// nothing
		}
		else if (height < 0.2) {
			if (slope < 0.4) {
				gkit_exp::add_mesh(veget, ms.getRandom_ByName("PalmTree"), model);
			}
			else {
				gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock"), model);
			}
		}
		else if (height < 0.6) {
			if (slope < 0.3) {
				if (wetness > 0.7) {
					if (random > 0.75) {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("Flowers"), model);
					}
					else if (random > 0.6) {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("Wheat"), model);
					}
					else {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("Grass"), model);
					}
				}
				else if (wetness > 0.5) {
					if (random > 0.75) {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("Bush"), model);
					}
					else if (random > 0.5) {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("BushBerries"), model);
					}
					else {
						gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock_Moss"), model);
					}
				}
				else {
					gkit_exp::add_mesh(veget, ms.getRandom_ByName("CommonTree"), model);
				}
			}
			else {
				gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock_Moss"), model);
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
				if (random > 0.2) {
					gkit_exp::add_mesh(veget, ms.getRandom_ByName("PineTree_Snow"), model);
				}
				else {
					gkit_exp::add_mesh(veget, ms.getRandom_ByName("Bush_Snow"), model);
				}
			}
			else {
				gkit_exp::add_mesh(veget, ms.getRandom_ByName("Rock_Snow"), model);
			}
		}
	}

	groups_veget = veget.groups();
}

