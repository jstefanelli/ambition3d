#include "ambition/ambition_api.h"

#include <semaphore>
#include <thread>
#include <iostream>
#include <gl/glew.h>
#include <SDL2/SDL.h>
#include <crlib/cc_task_scheduler.h>
#include <crlib/cc_task.h>
#include <ambition/renderTask.h>

std::shared_ptr<ambition::RenderTaskScheduler> ambition::RenderTaskScheduler::instance;

bool running = false;
void AMBITION_API Test(bool fullscreen) {
	ambition::RenderTaskScheduler::Instance();

	running = true;
	int res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_HAPTIC | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO);
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
	res = SDL_GetDisplayBounds(0, &display_bounds);
	if (res != 0) {
		std::cerr << "[SDL] Failed to get primary display bounds." << SDL_GetError() << std::endl;
		return;
	}

	SDL_Window* window;
	if (fullscreen) {
		window = SDL_CreateWindow("Ambition3D", display_bounds.x, display_bounds.y, display_bounds.w, display_bounds.h, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
	}
	else {
		window = SDL_CreateWindow("Ambition3D", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_OPENGL);
	}
	if (window == nullptr) {
		std::cerr << "[SDL] Failed to create window" << SDL_GetError() << std::endl;
	}

	SDL_ShowWindow(window);
	SDL_GLContext glContext = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, nullptr);

	std::thread renderThread([glContext, window]() -> void {
		int res = SDL_GL_MakeCurrent(window, glContext);
		if (res != 0) {
			std::cerr << "[Render] glMakeCurrent error: " << SDL_GetError() << std::endl;
			return;
		}
		glewInit();

		while (running) {
			auto task = ambition::RenderTaskScheduler::Instance()->task_queue.Pull();
			if (task.has_value()) {
				task.value().resume();
			}
		}
	});

	SDL_Event event;
	std::shared_ptr<std::binary_semaphore> frame_completed_semaphore = std::make_shared<std::binary_semaphore>(0);
	std::shared_ptr<float[]> rgb(new float[3]);
	rgb[0] = 0.0f;
	rgb[1] = 0.5f;
	rgb[2] = 1.0f;

	std::shared_ptr<int[]> up(new int[3]);
	up[0] = true;
	up[1] = true;
	up[2] = true;

	do {

		[rgb, up, window, frame_completed_semaphore]() -> ambition::RenderTask_t<int> {
			for (int i = 0; i < 3; i++) {
				float v = rgb[i];
				bool u = up[i];

				if (v >= 1.0f) {
					u = false;
				}
				else if (v <= 0) {
					u = true;
				}
				up[i] = u;

				if (u) {
					rgb[i] += 0.005f;
				}
				else {
					rgb[i] -= 0.005f;
				}
			}

			glClearColor(rgb[0], rgb[1], rgb[2], 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			SDL_GL_SwapWindow(window);
			frame_completed_semaphore->release();
			co_return 0;
		}();

		while(SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
				break;
			}
		}

		frame_completed_semaphore->acquire();
	} while(running);

	renderThread.join();
	SDL_Quit();
}