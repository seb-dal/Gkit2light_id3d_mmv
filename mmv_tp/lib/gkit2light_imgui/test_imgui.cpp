#include "test_imgui.h"
#include "wavefront.h"
#include "draw.h"

test_imgui::test_imgui(const int width, const int height, const int major, const int minor, const int samples) :
	App_imgui(width, height, major, minor, samples)
{}

test_imgui::~test_imgui()
{}



int test_imgui::init() {
	m_objet = read_mesh("data/bigguy.obj");

	Point pmin, pmax;
	m_objet.bounds(pmin, pmax);
	m_camera.lookat(pmin * 10, pmax * 10);

	glClearColor(
		clear_color.x * clear_color.w,
		clear_color.y * clear_color.w,
		clear_color.z * clear_color.w,
		clear_color.w
	);

	glClearDepth(1.f);                          // profondeur par defaut
	glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
	glEnable(GL_DEPTH_TEST);                    // activer le ztest

	return 0;
}



int test_imgui::quit() {

	m_objet.release();

	return 0;
}



int test_imgui::update(const float time, const float delta) {
	if (!ImGui::GetIO().WantCaptureMouse) {
		int mx, my;
		unsigned int mb = SDL_GetRelativeMouseState(&mx, &my);

		if (mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
			m_camera.rotation(mx, my);
		else if (mb & SDL_BUTTON(3))         // le bouton droit est enfonce
			m_camera.move(mx);
		else if (mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
			m_camera.translation((float)mx / (float)window_width(), (float)my / (float)window_height());
	}
	return 0;
}



int test_imgui::render_UI() {
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	return 0;
}



int test_imgui::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Transform rotT = RotationY(global_time() / 10.f);

	//draw(m_objet, Identity(), m_camera.view(), m_camera.projection());
	int i, j, l, k = 1;
	for (i = -k; i <= k; i++) {
		for (j = -k; j <= k; j++) {
			for (l = -k; l <= k; l++) {
				draw(m_objet, Translation(Vector(i, l, j) * 20.0) * rotT, m_camera.view(), m_camera.projection());
			}
		}
	}

	return 1;
}
