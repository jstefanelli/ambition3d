#include "ambition/logic/camera.h"
#include "ambition/logic/element.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/vec_swizzle.hpp>

namespace ambition::logic {
	crlib::Task Camera::LateUpdate(float) {
		auto matrix = RealElement()->UpdateTransform().FullModel();

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

    glm::mat4 Camera::ProjectionMatrix() {
        return last_projection_matrix;
    }

    Camera::Camera(const std::shared_ptr<Element>& element) : Component(element),
        near(0.01f),
        far(300.0f),
        projection_ok(false),
        last_view_matrix(1.0f),
        last_projection_matrix(1.0f)
    {

    }

    bool PerspectiveCamera::UpdateProjectionMatrix() {
        if (projection_ok)
            return false;


        last_projection_matrix = glm::perspective(fov, aspect_ratio, near, far);
        projection_ok = true;
        return true;
    }

    float PerspectiveCamera::FoV() const {
        return fov;
    }

    float PerspectiveCamera::AspectRatio() const {
        return aspect_ratio;
    }

    void PerspectiveCamera::FoV(float value_rad) {
        this->fov = value_rad;
        projection_ok = false;
    }

    void PerspectiveCamera::AspectRatio(float value) {
        this->aspect_ratio = value;
        projection_ok = false;
    }
}