#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace ambition {
	class Transform {
	protected:
		glm::vec3 position;
		glm::quat orientation;
		glm::vec3 scale;

		bool local_up_to_date;
		bool full_up_to_date;

		glm::mat4 parent;
		glm::mat4 model_local;
		glm::mat4 model_full;
	public:
		Transform();
		Transform(const Transform& other);

		Transform& operator=(const Transform& other);

		void Position(const glm::vec3& position);
		void Orientation(const glm::quat& ori);
		void Scale(const glm::vec3& scl);
		void Parent(const glm::mat4& pr);

		glm::vec3 Position() const;
		glm::quat Orientation() const;
		glm::vec3 Scale() const;
		glm::mat4 Parent() const;

		bool Update();
		glm::mat4 LocalModel();
		glm::mat4 FullModel();
	};
}