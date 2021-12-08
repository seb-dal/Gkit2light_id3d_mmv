#pragma once
#include "app_imgui.h"
#include "mesh.h"
#include "orbiter.h"

class test_imgui :public App_imgui {
public:
	test_imgui(const int width, const int height, const int major = 3, const int minor = 3, const int samples = 0);

	virtual ~test_imgui();

	bool show_demo_window = false;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	int init();

	int quit();

	int update(const float time, const float delta);
	//int update(const float time, const float delta);
	int render_UI();

	int render();



protected:

	Mesh m_objet;
	GLuint m_texture;
	Orbiter m_camera;
};

