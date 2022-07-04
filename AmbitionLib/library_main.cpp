#include "ambition/ambition_api.h"

#include <semaphore>
#include <thread>
#include <iostream>
#include <gl/glew.h>
#include <SDL2/SDL.h>
#include <crlib/cc_task_scheduler.h>
#include <crlib/cc_task.h>
#include <ambition/renderTask.h>
#include <ambition/gl/shader.h>
#include <ambition/gl/buffer.h>
#include <ambition/gl/texture.h>
#include <ambition/gl/utils.h>
#include <ambition/resources/texture.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtx/transform.hpp>

std::shared_ptr<ambition::RenderTaskScheduler> ambition::RenderTaskScheduler::instance;

auto vertex_shader = R"(#version 330
in vec3 data;
in vec4 uv;

out vec4 v_uv;
uniform mat4 mvp;

void main() {
	gl_Position = mvp * vec4(data, 1.0);
	v_uv = uv;
}
)";

auto fragment_shader = R"(
#version 330
in vec4 v_uv;

uniform sampler2D txt;

out vec4 o_color;

void main() {
	o_color = texture(txt, v_uv.xy);
}
)";

float vertex_data[]{
	-1, -1, -1,
	1, -1, -1,
	1, 1, -1,
	-1, 1, -1
};

short indices[]{
	0, 1, 2, 0, 2, 3
};

glm::vec4 uv_data[]{
	glm::vec4(0, 0, 0, 0),
	glm::vec4(1, 0, 1, 0),
	glm::vec4(1, 1, 1, 1),
	glm::vec4(0, 1, 0, 1)
};

glm::ivec4 colors[]{
	glm::ivec4(255, 0, 0, 255),
	glm::ivec4(0, 255, 0, 255),
	glm::ivec4(0, 0, 255, 255)
};

glm::ivec2 sizes[]{
	glm::ivec2(4, 4),
	glm::ivec2(2, 2),
	glm::ivec2(1, 1)
};

bool running = false;
GLint uv_id = 0, vertex_id = 0, txt_id = 0, mvp_loc = 0;
GLuint vao = 0;
int loadedLevel = 2;
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
			std::coroutine_handle<> h;
			if (ambition::RenderTaskScheduler::Instance()->task_queue.pop(h)) {
				h.resume();
			}
		}
	});

    auto render_thread_id = renderThread.get_id();

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

	std::vector<std::shared_ptr<ambition::resources::Texture2D>> textures;
	for (int i = 0; i < 3; i++) {
		auto color = colors[i];
		auto size = sizes[i];

		std::vector<unsigned char> data(size.x * size.y * 4);
		for (auto p = 0; p < size.x * size.y; p++) {
			data[(p * 4) + 0] = (unsigned char)color.x;
			data[(p * 4) + 1] = (unsigned char)color.y;
			data[(p * 4) + 2] = (unsigned char)color.z;
			data[(p * 4) + 3] = (unsigned char)color.a;
		}

		auto t = std::make_shared<ambition::resources::Texture2D>(data, ambition::resources::PixelFormat::RGBA, ambition::resources::PixelType::BYTE, size);
		textures.push_back(std::move(t));
	}
	
	bool loaded = false;
    auto last_time = std::chrono::high_resolution_clock::now();
	auto shader = std::shared_ptr<ambition::gl::Shader>();
	auto last_load_time = std::chrono::high_resolution_clock::now();
	std::shared_ptr<ambition::gl::Texture2D> glTexture;
	std::shared_ptr<ambition::gl::Buffer<GL_ARRAY_BUFFER>> vertex_buffer;
	std::shared_ptr<ambition::gl::Buffer<GL_ARRAY_BUFFER>> uv_buffer;
	do {

		if (loaded) {
			[rgb, up, window, frame_completed_semaphore, render_thread_id, &last_time, &last_load_time, glTexture, &textures]() -> ambition::RenderTask_t<int> {
				auto now = std::chrono::high_resolution_clock::now();

				auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);
				last_time = now;

				if (std::this_thread::get_id() != render_thread_id) {
					std::cerr << "[RenderTask] Running on the wrong thread." << std::endl;
					co_return 1;
				}

				auto load_delta = std::chrono::duration_cast<std::chrono::seconds>(now - last_load_time);
				if (load_delta > std::chrono::seconds(5) && loadedLevel > 0) {
					loadedLevel--;
					std::cout << "[RenderTask] Loading next MIP level..." << std::endl;
					co_await glTexture->Load(textures[loadedLevel], loadedLevel, false);
					last_load_time = now;
				}


				//std::cout << "[GL] Delta time: " << delta.count() << std::endl;

				/*
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
				*/

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				CHECK_GL();


				glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
				CHECK_GL();


				SDL_GL_SwapWindow(window);
				frame_completed_semaphore->release();

				co_return 2;
			}();
		}
		else {
			auto x = [window, &loaded, &glTexture, &textures, frame_completed_semaphore, &last_time, &shader, &vertex_buffer, &uv_buffer]() -> ambition::RenderTask_t<bool> {
				auto now = std::chrono::high_resolution_clock::now();

				auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time);
				last_time = now;

				std::cout << "[GL] Delta time: " << delta.count() << std::endl;

				std::cout << "[GL] Loading..." << std::endl;

				glTexture = std::make_shared<ambition::gl::Texture2D>();
				co_await glTexture->Allocate(textures[0]->Size(), textures[0]->PixelFormat(), textures[0]->PixelType());
				co_await glTexture->Load(textures[2], 2, false);
				shader = std::make_shared<ambition::gl::Shader>();
				auto res = co_await shader->Load(vertex_shader, fragment_shader);

				glUseProgram(shader->Id());
				CHECK_GL();

				vertex_id = glGetAttribLocation(shader->Id(), "data");
				CHECK_GL();
				uv_id = glGetAttribLocation(shader->Id(), "uv");
				CHECK_GL();
				txt_id = glGetUniformLocation(shader->Id(), "txt");
				CHECK_GL();
				mvp_loc = glGetUniformLocation(shader->Id(), "mvp");
				CHECK_GL();

				glm::mat4 mvp = glm::perspective(60.0f, 16.0f / 9.0f, 0.001f, 100.0f);

				vertex_buffer = ambition::gl::Buffer<GL_ARRAY_BUFFER>::Generate();
				uv_buffer = ambition::gl::Buffer<GL_ARRAY_BUFFER>::Generate();

				std::vector<float> v_data;
				std::copy(&vertex_data[0], &vertex_data[12], std::back_inserter(v_data));
				co_await vertex_buffer->Load(v_data);

				std::vector<glm::vec4> u_data;
				std::copy(&uv_data[0], &uv_data[4], std::back_inserter(u_data));
				co_await uv_buffer->Load(u_data);

				glCreateVertexArrays(1, &vao);
				CHECK_GL();

				glBindVertexArray(vao);
				CHECK_GL();

				glEnableVertexAttribArray(vertex_id);
				CHECK_GL();

				co_await vertex_buffer->Bind();
				glVertexAttribPointer(vertex_id, 3, GL_FLOAT, false, 0, nullptr);
				CHECK_GL();

				glEnableVertexAttribArray(uv_id);
				CHECK_GL();

				co_await uv_buffer->Bind();
				glVertexAttribPointer(uv_id, 4, GL_FLOAT, false, 0, nullptr);
				CHECK_GL();

				glActiveTexture(GL_TEXTURE0);
				CHECK_GL();
				glBindTexture(GL_TEXTURE_2D, glTexture->Id());
				CHECK_GL();

				glUniform1i(txt_id, 0);
				CHECK_GL();


				glUniformMatrix4fv(mvp_loc, 1, false, &mvp[0][0]);
				CHECK_GL();

				loaded = true;
				SDL_GL_SwapWindow(window);
				frame_completed_semaphore->release();
				co_return res;
			}().wait();

			if (!x) {
				renderThread.join();
				SDL_Quit();
				return;
			}
		}

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