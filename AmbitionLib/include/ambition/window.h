#pragma once
#include <SDL2/SDL.h>
#include <gl/glew.h>
#include <glm/glm.hpp>
#include <memory>
#include "./gl/renderer.h"

namespace ambition {
	enum class WindowMode {
		Windowed = 0,
		ExclusiveFullscreen = 1,
		BorderlessFullscreen = 2
	};

	class Window {
	protected:
		static bool sdl_initialized;
		static void InitSDL();

		std::weak_ptr<Window> self;

		SDL_Window* window;
		bool window_visible;
		SDL_GLContext gl_context;

		glm::ivec2 current_window_size;
		WindowMode current_window_mode;
		bool vsync;
		Window(glm::ivec2 size, WindowMode mode);
	public:
		static std::shared_ptr<Window> Make();

		Window() = delete;
		~Window();

		void Show();
		void Hide();
		bool Visible() const;

		void Mode(WindowMode m);
		WindowMode Mode() const;

		void Vsync(bool enabled);
		bool VSync() const;
	};
}