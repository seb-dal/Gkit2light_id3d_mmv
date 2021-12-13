#include "MMV_Viewer.h"

#include "../vecPlus/gkit_expension.h"


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


	//veget = read_mesh("./id3d_mmv/data/Ultimate Nature Pack/OBJ/CommonTree/CommonTree_1.obj");

	//Image img(read_image("./id3d_mmv/mmv_tp/data/island.jpg"));  //a.jpg
	Image img(read_image("./id3d_mmv/mmv_tp/data/a.jpg"));


	texture = Image(width_map, height_map, White());
	gl_texture = make_texture(0, texture);

	Point pm, pM;
	pM.x = width_map * 2.5;
	pM.y = 64;
	pM.z = height_map * 2.5;

	map_box = BBox(pm, pM);
	hf = HeightField(img, map_box, Coord2(width_map, height_map), vec2(1, 1));
	//hf = HeightField(nc, BBox(pm, pM), Coord2(width_map, height_map), vec2(1, 1));


	terrain = hf.to_Mesh(p);
	Builder::Compute_normal(terrain);

	water = Builder::flat_Mesh(width_map, height_map, map_box, pw);
	//hf.updateMesh_normal(m);
	//m_camera.setFOV(75);
	m_camera.projection(window_width(), window_height(), 75);
	m_camera.lookat(pm, pM);
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




int MMV_Viewer::update(const float time, const float delta) {
	if (!ImGui::GetIO().WantCaptureMouse) {
		m_camera.update(delta);
	}

	if (render_water) {
		const auto& pos = water.positions(), & pos2 = terrain.positions();
		float time_speed = time * 0.01;
#pragma omp parallel for
		for (int i = 0; i < water.vertex_count(); i++) {
			if (pos2[i].y < map_box.pmax.y * min_water_palier) {
				Point p = pos[i];
				p.y = map_box.pmax.y * min_water + Water_noise.getHeight(p.x + time_speed, p.z + time_speed) * height_water + water_level;
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
