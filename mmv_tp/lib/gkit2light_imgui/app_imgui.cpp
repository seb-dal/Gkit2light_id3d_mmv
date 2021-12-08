
//! \file app.cpp

#include <cstdio>
#include <cstring>

#include "app_imgui.h"
#include "glcore.h"
#include <vector>

#include "./IMGUI/imgui_internal.h"


App_imgui::App_imgui(const int width, const int height, const int major, const int minor, const int samples)
	: m_window(nullptr), m_context(nullptr)
{
	m_window = create_window(width, height, major, minor, samples);
	m_context = create_context(m_window);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls


		// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(m_window, m_context);

#if defined(IMGUI_IMPL_OPENGL_ES2)
	// GL ES 2.0 + GLSL 100
	const char* glsl_version = "#version 100";
#elif defined(__APPLE__)
	// GL 3.2 Core + GLSL 150
	const char* glsl_version = "#version 150";
#else
	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
#endif

	ImGui_ImplOpenGL3_Init(glsl_version);
}

App_imgui::~App_imgui()
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	if (m_context)
		release_context(m_context);

	if (m_window)
		release_window(m_window);
}

int App_imgui::run()
{
	if (init() < 0)
		return -1;

	// configure openGL
	glViewport(0, 0, window_width(), window_height());

	// gestion des evenements
	while (events(m_window))
	{
		if (prerender() < 0)
			break;

		if (render() < 1)
			break;

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		if (postrender() < 0)
			break;

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// presenter le resultat
		SDL_GL_SwapWindow(m_window);

		// force openGL a finir d'executer toutes les commandes, 
		// cf https://www.khronos.org/opengl/wiki/Swap_Interval#GPU_vs_CPU_synchronization
		// devrait limiter la consommation sur portable
		glFinish();

		SDL_Event event;
		std::vector<SDL_Event> list_events;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			list_events.push_back(event);
		}

		for (SDL_Event e : list_events) {
			SDL_PushEvent(&e);
		}
	}

	if (quit() < 0)
		return -1;

	// tout c'est bien passe...
	return 0;
}
