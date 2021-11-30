#pragma once

#include "orbiter.h"
#include "window.h"
#include "mesh.h"
#include "draw.h"




class Camera : public Orbiter {
	static void GL_draw_axis(Mesh& m, float sizeAxe);

public:
	Camera();

	void setFOV(float fov);

	// update camera
	virtual void update(const float delta);

	// save state camera
	virtual void saveCamera();

	// load saved camera state
	virtual void loadCamera();

	// draw axes center view
	void draw_axes();

	// destroy mesh axes center view
	virtual void destroy();


public:
	SDL_Keycode key_reset = 'r';

	SDL_Keycode key_changeMode = 'c';
	SDL_Keycode key_forward = 'z';
	SDL_Keycode key_backward = 's';
	SDL_Keycode key_left = 'q';
	SDL_Keycode key_right = 'd';
	SDL_Keycode key_up = ' ';
	SDL_Keycode key_down = SDLK_LSHIFT;
	SDL_Keycode key_speedup = SDLK_LCTRL;

	float move_speed = 0.5;
	float move_speedup = 3;
protected:
	bool mode_Orbiter;

	Mesh Axes;
	Orbiter ob;
private:
	float m_size_orbiteur;

	bool SAVE_mode_Orbiter;

	Point SAVE_m_center;
	vec2 SAVE_m_position;
	vec2 SAVE_m_rotation;
	float SAVE_m_size;
	float SAVE_m_radius;

	float SAVE_m_width;
	float SAVE_m_height;
	float SAVE_m_fov;
	Orbiter SAVE_ob;
};
