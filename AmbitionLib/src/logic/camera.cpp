#include "ambition/logic/camera.h"
#include "ambition/logic/element.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/vec_swizzle.hpp>

namespace ambition::logic {
	crlib::Task Camera::LateUpdate(float delta) {
		auto matrix = Element()->UpdateTransform().FullModel();

		glm::vec3 translation = matrix[3];
		glm::vec3 forward = glm::xyz(matrix * glm::vec4(0, 0, -1, 1));
		glm::vec3 up = glm::xyz(glm::normalize(matrix * glm::vec4(0, 1, 0, 1)));

		last_view_matrix = glm::lookAtRH(translation, forward, up);

		co_return;
	}

	glm::mat4 Camera::ViewMatrix() {
		return last_view_matrix;
	}

	float Camera::Near() const {
		return near;
	}

	float Camera::Far() const {
		return far;
	}

	void Camera::Near(float near) {
		this->near = near;
		projection_ok = false;
	}

	void Camera::Far(float far) {
		this->far = far;
		projection_ok = false;
	}
}