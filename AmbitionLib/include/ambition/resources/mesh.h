#pragma once
#include "resource.h"
#include <vector>
#include <glm/glm.hpp>

namespace ambition::resources {
	enum class FaceType {
		POINTS = 1,
		LINES = 2,
		TRIANGLES = 3,
		TRIANGLE_FANS = 4,
		TRIANGLE_STRIPS = 5,
		QUADS = 6,
		PATCHES = 7,
	};

	constexpr int VertexPerFace(FaceType t) {
		switch(t) {
			case FaceType::POINTS:
				return 1;
			case FaceType::LINES:
				return 2;
			case FaceType::TRIANGLES:
			case FaceType::TRIANGLE_FANS:
			case FaceType::TRIANGLE_STRIPS:
				return 3;
			case FaceType::QUADS:
				return 4;
			case FaceType::PATCHES:
			default:
				return 0;
		}
	}

	class Mesh : public Resource {
	protected:
		std::vector<glm::vec3> vertices;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec4> packedUVs;
		std::vector<glm::vec3> tangents;
		std::vector<glm::vec3> biTangents;
		std::vector<glm::vec4> vertexColors;

		FaceType faceType;
		~Mesh() override = default;
	public:
		virtual size_t DrawAmount() const;

		FaceType FaceType() const;
		const std::vector<glm::vec3>& Vertices() const;
		const std::vector<glm::vec3>& Normals() const;
		const std::vector<glm::vec4>& PackedUVs() const;
		const std::vector<glm::vec3>& Tangents() const;
		const std::vector<glm::vec3>& BiTangents() const;
		const std::vector<glm::vec4>& VertexColors() const;
	};
}