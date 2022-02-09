#pragma once

#include "component.h"
#include <glm/glm.hpp>

namespace ambition {
	namespace logic {
		class Camera : public Component {
		protected:
			float near;
			float far;
			bool projection_ok;
		public:
			virtual glm::mat4 ProjectionMatrix() = 0;
			virtual glm::mat4 ViewMatrix();

			float Near() const;
			float Far() const;
			virtual void Near(float val);
			virtual void Far(float val);
		};

		class PerspectiveCamera : public Camera {
		protected:
			float fov;
			float aspect_ratio;
		public:
			virtual glm::mat4 ProjectionMatrix() override;
			float FoV() const;
			float AspectRatio() const;
			void FoV(float value_rad);
			void AspectRatio(float value);
		};
	}
}