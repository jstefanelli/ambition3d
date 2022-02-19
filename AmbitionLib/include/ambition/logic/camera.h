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

			glm::mat4 last_view_matrix;
			glm::mat4 last_projection_matrix;

            Camera(const std::shared_ptr<Element>& element);
            Camera(const Camera& other) = default;
			virtual bool UpdateProjectionMatrix() = 0;
		public:
			virtual crlib::Task LateUpdate(float delta) override;
			virtual glm::mat4 ProjectionMatrix();
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

			bool UpdateProjectionMatrix() override;
		public:
			float FoV() const;
			float AspectRatio() const;
			void FoV(float value_rad);
			void AspectRatio(float value);
		};
	}
}