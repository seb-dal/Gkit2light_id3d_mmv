#include "Camera.h"

static const float CONVERT_DEG2RAD = M_PI / 180.f;

static float sin_d(float deg) {
	return sin(deg * CONVERT_DEG2RAD);
}
static float cos_d(float deg) {
	return cos(deg * CONVERT_DEG2RAD);
}



void Camera::GL_draw_axis(Mesh& m, float sizeAxe) {
	m = Mesh(GL_LINES);
	m.color(Red());
	m.vertex(Point());
	m.vertex(Point(sizeAxe, 0, 0));

	m.color(Green());
	m.vertex(Point());
	m.vertex(Point(0, sizeAxe, 0));

	m.color(Blue());
	m.vertex(Point());
	m.vertex(Point(0, 0, sizeAxe));
}





Camera::Camera() :Orbiter() {
	SDL_SetRelativeMouseMode(SDL_FALSE);
	mode_Orbiter = true; // Mode par defaut, ORBITER
	m_size_orbiteur = m_size;
	saveCamera();

	Axes = Mesh(GL_LINES);
	GL_draw_axis(Axes, 0.2);
}

void Camera::setFOV(float fov) { m_fov = fov; }


void Camera::switch_view_mode() {
	change_view_mode(!mode_Orbiter);
}


void Camera::change_view_mode(bool mode_orbiteur) {
	mode_Orbiter = mode_orbiteur;

	if (mode_orbiteur) {
		m_size = m_size_orbiteur;
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
	else {
		m_size_orbiteur = m_size;
		m_size = 0;
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
}





// update camera
void Camera::update(const float delta) {
	float delta_T = 1000.f / 60.f;
	if (delta < 1000.f) {
		delta_T = delta / 60.f;
	}

	if (key_state(key_reset)) {
		clear_key_state(key_reset);
		loadCamera();
	}
	projection(window_width(), window_height(), m_fov);

	//changement de mode de camera
	if (key_state(key_changeMode)) {
		clear_key_state(key_changeMode);

		switch_view_mode();
	}

	int mx, my;
	unsigned int mb = SDL_GetRelativeMouseState(&mx, &my);
	int mousex, mousey;// recupere les mouvements de la souris
	SDL_GetMouseState(&mousex, &mousey);


	SDL_MouseWheelEvent wheel = wheel_event();
	if (wheel.y != 0) {
		clear_wheel_event();
		move(8.f * wheel.y);  // approche / eloigne l'objet
	}

	if (mb & SDL_BUTTON(2))
		move(mx);



	if (mode_Orbiter) {
		if (mb & SDL_BUTTON(1)) {
			// tourne autour de l'objet
			rotation(mx, my);
			ob.rotation(mx, my);
		}
		// deplace la m_camera
		if (mb & SDL_BUTTON(3))
			translation((float)mx / (float)window_width(), (float)my / (float)window_height()); // deplace le point de rotation

	}
	else { //Mode camera libre
		rotation(mx * 0.875f, my * 0.875f);
		ob.rotation(mx * 0.875f, my * 0.875f);

		const float
			sx = sin_d(m_rotation.x),
			sy = sin_d(m_rotation.y),
			cx = cos_d(m_rotation.x),
			cy = cos_d(m_rotation.y),
			speedup = (key_state(key_speedup)) ? move_speedup : 1
			;


		// on deplace le centre de la camera en fonction de la rotation de la camera
		m_center = m_center
			+
			Vector( // Deplacement vers l'avant/arriere
				sy * cx * move_speed,
				-sx * move_speed,
				-cy * cx * move_speed
			) * (key_state(key_forward) - key_state(key_backward)) * speedup * delta_T
			+
			Vector( // Deplacement vers la droit/gauche
				cy * move_speed,
				0,
				sy * move_speed
			) * (key_state(key_right) - key_state(key_left)) * speedup * delta_T
			+
			Vector( // Deplacement vers le haut/bas
				0,
				move_speed,
				0
			) * (key_state(key_up) - key_state(key_down)) * speedup * delta_T
			;

	}
}





// save state camera
void Camera::saveCamera() {
	SAVE_mode_Orbiter = mode_Orbiter;

	SAVE_m_center = m_center;
	SAVE_m_position = m_position;
	SAVE_m_rotation = m_rotation;

	SAVE_m_size = m_size;
	SAVE_m_radius = m_radius;

	SAVE_m_fov = m_fov;
	SAVE_m_height = m_height;
	SAVE_m_width = m_width;

	SAVE_ob = Orbiter(ob);
}





// load saved camera state
void Camera::loadCamera() {
	if (SAVE_mode_Orbiter) {
		m_size = m_size_orbiteur;
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}
	else {
		m_size_orbiteur = m_size;
		m_size = 0;
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
	mode_Orbiter = SAVE_mode_Orbiter;

	m_center = SAVE_m_center;
	m_position = SAVE_m_position;
	m_rotation = SAVE_m_rotation;

	m_size = SAVE_m_size;
	m_radius = SAVE_m_radius;

	m_fov = SAVE_m_fov;
	m_height = SAVE_m_height;
	m_width = SAVE_m_width;

	ob = Orbiter(SAVE_ob);
}





// draw axes center view
void Camera::draw_axes() {
	glDepthFunc(GL_ALWAYS);
	draw(Axes, ob);
	glDepthFunc(GL_LEQUAL);
}





// destroy mesh axes center view
void Camera::destroy() {
	Axes.clear();
	Axes.release();
}


