#include "ambition/transform.h"
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtx/quaternion.hpp>

namespace ambition {
	Transform::Transform() : position(0, 0, 0), orientation(), scale(1, 1, 1), 
		model_local(1.0f), model_full(1.0f), parent(1.0f),
		local_up_to_date(false), full_up_to_date(false) {
		
	}

	Transform::Transform(const Transform& other) : position(other.position), orientation(other.orientation), scale(other.scale),
		model_local(other.model_local), model_full(other.model_full), parent(other.parent),
		local_up_to_date(other.local_up_to_date), full_up_to_date(other.full_up_to_date) {

	}

	Transform& Transform::operator=(const Transform& other) {
		position = other.position;
		orientation = other.orientation;
		scale = other.scale;
		model_local = other.model_local;
		model_full = other.model_full;
		parent = other.parent;
		local_up_to_date = other.local_up_to_date;
		full_up_to_date = other.full_up_to_date;
		return *this;
	}

	void Transform::Position(const glm::vec3& pos) {
		position = pos;
		local_up_to_date = false;
		full_up_to_date = false;
	}

	void Transform::Orientation(const glm::quat& ori) {
		orientation = ori;
		local_up_to_date = false;
		full_up_to_date = false;
	}

	void Transform::Scale(const glm::vec3& scl) {
		scale = scl;
		local_up_to_date = false;
		full_up_to_date = false;
	}

	void Transform::Parent(const glm::mat4& p) {
		parent = p;
		full_up_to_date = false;
	}

	glm::vec3 Transform::Position() const {
		return position;
	}

	glm::quat Transform::Orientation() const {
		return orientation;
	}

	glm::vec3 Transform::Scale() const {
		return scale;
	}

	glm::mat4 Transform::Parent() const {
		return parent;
	}

	bool Transform::Update() {
		if (!local_up_to_date || !full_up_to_date) {
			FullModel();
			return true;
		}

		return false;
	}

	glm::mat4 Transform::LocalModel() {
		if (!local_up_to_date) {
			model_local = glm::translate(glm::mat4(1.0), position);
			model_local = model_local * glm::toMat4(orientation);
			model_local = glm::scale(model_local, scale);
			local_up_to_date = true;
		}

		return model_local;
	}

	glm::mat4 Transform::FullModel() {
		if (!full_up_to_date) {
			model_full = parent * LocalModel();
			full_up_to_date = true;
		}

		return model_full;
	}
}