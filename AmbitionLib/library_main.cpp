#include "ambition/ambition_api.h"

#include <iostream>
#include <gl/glew.h>
#include <SDL2/SDL.h>

void AMBITION_API Test(bool fullscreen) {

	int res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);
	if (res != 0) {
		std::cerr << "[SDL] Initialization failed: " << SDL_GetError() << std::endl;
		return;
	}

	SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GLattr::SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	SDL_Rect display_bounds;
	res = SDL_GetDisplayBounds(0, & display_bounds);
	if (res != 0) {
		std::cerr << "[SDL] Failed to get primary display bounds." << SDL_GetError() << std::endl;
		return;
	}

	SDL_Window* window;
	if (fullscreen) {
		window = SDL_CreateWindow("Ambition3D", display_bounds.x, display_bounds.y, display_bounds.w, display_bounds.h, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
	} else {
		window = SDL_CreateWindow("Ambition3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	}
	if (window == nullptr) {
		std::cerr << "[SDL] Failed to create window" << SDL_GetError() << std::endl;
	}

	SDL_ShowWindow(window);

	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	glewInit();

	bool running = true;
	SDL_Event event;

	float rgb[] { 0.0f, 0.5f, 1.0f };
	bool up[] { true, true, true };
	do {
		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				break;
			}
		}

		for (int i = 0; i < 3; i++) {
			float v = rgb[i];
			bool u = up[i];

			if (v >= 1.0f) {
				u = false;
			} else if (v <= 0) {
				u = true;
			}
			up[i] = u;

			if (u) {
				rgb[i] += 0.005f;
			} else {
				rgb[i] -= 0.005f;
			}
		}

		glClearColor(rgb[0], rgb[1], rgb[2], 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		SDL_GL_SwapWindow(window);
	} while(running);

	SDL_Quit();
}