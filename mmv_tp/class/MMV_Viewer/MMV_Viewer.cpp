#include "MMV_Viewer.h"

#include "../vecPlus/gkit_expension.h"


void MMV_Viewer::load_water(int width, int height) {
	if (water.vertex_count() > 0) {
		water.clear();
	}

	X_Water = width;
	Z_Water = height;

	water = Builder::flat_Mesh(X_Water, Z_Water, World_box, pw);
}



void MMV_Viewer::load_texture(int width, int height) {
	if (texture.size() > 0) {
		glDeleteTextures(1, &gl_texture);
	}

	X_texture = width;
	Y_texture = height;

	texture = Image(X_texture, Y_texture, White());
	gl_texture = make_texture(0, texture);
}



void MMV_Viewer::load_height_map(const char* path, const Point& domain) {
	if (terrain.vertex_count() > 0) {
		terrain.clear();
	}

	Image img(read_image(path));

	Point pm, pM;
	pM.x = domain.x * 2.5;
	pM.y = domain.y;
	pM.z = domain.z * 2.5;
	World_box = BBox(pm, pM);

	hf = HeightField(img, World_box, Coord2(domain.x, domain.z), vec2(1, 1));
	//hf = HeightField(nc, BBox(pm, pM), Coord2(width_map, height_map), vec2(1, 1));


	terrain = hf.to_Mesh(p);
	Builder::Compute_normal(terrain);

	load_water(X_Water, Z_Water);

	m_camera.lookat(pm, pM);
}




void MMV_Viewer::init_noise() {
	FastNoiseLite a;
	a.SetNoiseType(FastNoiseLite::NoiseType::NoiseType_Cellular);
	a.SetFrequency(0.01);

	a.SetFractalType(FastNoiseLite::FractalType_PingPong);
	a.SetFractalOctaves(5);
	a.SetFractalLacunarity(2.7);
	a.SetFractalGain(0);
	a.SetFractalWeightedStrength(1.7);
	a.SetFractalPingPongStrength(0.1);

	a.SetCellularDistanceFunction(FastNoiseLite::CellularDistanceFunction_Euclidean);
	a.SetCellularReturnType(FastNoiseLite::CellularReturnType_Distance);

	nc.add(a);


	FastNoiseLite b;
	b.SetNoiseType(FastNoiseLite::NoiseType_ValueCubic);
	b.SetFrequency(0.015);

	b.SetFractalType(FastNoiseLite::FractalType_Ridged);
	b.SetFractalOctaves(3);
	b.SetFractalLacunarity(2.7);
	b.SetFractalGain(0.2);
	b.SetFractalWeightedStrength(1.7);

	nc.add(b);

	FastNoiseLite c;
	c.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	c.SetFrequency(0.005);

	nc.add(c);



	FastNoiseLite w;
	w.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
	w.SetFrequency(0.016);
	w.SetFractalType(FastNoiseLite::FractalType_FBm);
	w.SetFractalOctaves(5);
	w.SetFractalLacunarity(2);
	w.SetFractalGain(0.5);

	Water_noise.add(w);
}





int MMV_Viewer::init()
{
	glCullFace(true);

	init_noise();
	ms.load();


	load_texture(X_texture, Y_texture);
	load_height_map("./id3d_mmv/mmv_tp/data/a.jpg", Point(X_map, 64, Z_map));

	//veget = read_mesh("./id3d_mmv/data/Ultimate Nature Pack/OBJ/CommonTree/CommonTree_1.obj");

	//Image img(read_image("./id3d_mmv/mmv_tp/data/island.jpg"));  //a.jpg

	m_camera.projection(window_width(), window_height(), 75);
	m_camera.move_speed = 5;
	m_camera.move_speedup = 10;
	m_camera.saveCamera();


	// etat openGL par defaut
	glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre

	glClearDepth(1.f);                          // profondeur par defaut
	glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
	glEnable(GL_DEPTH_TEST);

	return 0;
}




int MMV_Viewer::quit()
{
	water.release();
	water.clear();

	terrain.release();
	terrain.clear();

	groups_veget.clear();
	veget.release();
	veget.clear();
	ms.destroy();

	m_camera.destroy();
	glDeleteTextures(1, &gl_texture);
	return 0;
}




void MMV_Viewer::update_Mesh() {
	hf.updateMesh(terrain);

	if (p.normal)
		Builder::Compute_normal(terrain);
}





void MMV_Viewer::update_texture() {
	glDeleteTextures(1, &gl_texture);
	gl_texture = make_texture(1, texture);
}


//static float time_x = 0;
//static float time_z = 0;

int MMV_Viewer::update(const float time, const float delta) {
	if (!ImGui::GetIO().WantCaptureMouse) {
		m_camera.update(delta);
	}

	if (render_water) {
		const auto
			& pos = water.positions();

		float time_speed = time * 0.01;

		float time_x = cos(time_speed * 0.01) * sin(0.368 + time_speed * 0.008) * 100;
		float time_z = sin(time_speed * 0.01) * cos(1.2358 + time_speed * 0.015) * 100;

#pragma omp parallel for
		for (int i = 0; i < water.vertex_count(); i++) {
			Point p = pos[i];
			Point pp = hf.pts_domain(p);
			if (hf.interp(pp.x, pp.z) < World_box.pmax.y * min_water_palier) {
				p.y = World_box.pmax.y * min_water + Water_noise.getHeight(p.x + time_x, p.z + time_z) * water_height_variation;
				water.vertex(i, p);
			}
		}
	}

	return 0;
}



int MMV_Viewer::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (render_water) {
		draw(water, m_camera);
	}

	draw(terrain, m_camera, gl_texture);

	if (render_veget) {
		gkit_exp::draw_groups(veget, Identity(), m_camera.view(), m_camera.projection(), groups_veget);
	}
	//draw(veget, Scale(20), m_camera);

	m_camera.draw_axes();
	return 1;
}


