#pragma once
#include <gl/glew.h>
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <crlib/cc_task.h>
#include <memory>

#include "../logic/scene.h"

namespace ambition {
	namespace gl {
		class Renderer {
		protected:
			glm::ivec2 target_resolution;
			glm::ivec2 current_resolution;

			std::shared_ptr<logic::Scene> current_scene;
			std::shared_ptr<logic::Scene> next_scene;

		public:
			glm::ivec2 CurrentResolution() const;
			glm::ivec2 TargetResolution() const;
			void TargetResolution(glm::ivec2 res);

			std::shared_ptr<logic::Scene> CurrentScene() const;
			std::shared_ptr<logic::Scene> NextSceene() const;
			void NextScene(std::shared_ptr<logic::Scene> scene);
		};
	}
}