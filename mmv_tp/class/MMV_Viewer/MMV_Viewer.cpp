#include "MMV_Viewer.h"

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
}


int MMV_Viewer::init()
{
	init_noise();
	init_functions();

	int height_map = 400,
		width_map = 400;




	Image img(read_image("./id3d_mmv/mmv_tp/data/island.jpg"));

	texture = Image(width_map, height_map, Green());
	gl_texture = make_texture(0, texture);

	Point pm, pM;
	pM.x = 1400;
	pM.y = 500;
	pM.z = 1400;


	hf = HeightField(img, BBox(pm, pM), Coord2(width_map, height_map), vec2(1, 1));
	//hf = HeightField(nc, BBox(pm, pM), Coord2(width_map, height_map), vec2(1, 1));


	m = hf.to_Mesh(p);
	Builder::Compute_normal(m);
	//hf.updateMesh_normal(m);
	m_camera.setFOV(75);
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
	m.release();
	m.clear();
	m_camera.destroy();
	glDeleteTextures(1, &gl_texture);
	return 0;
}




void MMV_Viewer::update_Mesh() {
	hf.updateMesh(m);
	Builder::Compute_normal(m);
	//hf.updateMesh_normal(m);
}




void MMV_Viewer::update_texture() {
	glDeleteTextures(1, &gl_texture);
	gl_texture = make_texture(1, texture);
}

int MMV_Viewer::update(const float time, const float delta) {
	m_camera.update(delta);

	for (const auto& f : functions) {
		f();
	}


	return 0;
}

int MMV_Viewer::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	draw(m, m_camera, gl_texture);

	m_camera.draw_axes();
	return 1;
}
