#include <ambition/resources/mesh.h>

namespace ambition::resources {
	size_t Mesh::DrawAmount() const {
		return vertices.size();
	}

	const std::vector<glm::vec3> &Mesh::Vertices() const {
		return vertices;
	}

	const std::vector<glm::vec3>& Mesh::Normals() const {
		return normals;
	}

	const std::vector<glm::vec4> &Mesh::PackedUVs() const {
		return packedUVs;
	}

	const std::vector<glm::vec3> &Mesh::Tangents() const {
		return tangents;
	}

	const std::vector<glm::vec3> &Mesh::BiTangents() const {
		return biTangents;
	}

	const std::vector<glm::vec4> &Mesh::VertexColors() const {
		return vertexColors;
	}

	FaceType Mesh::FaceType() const {
		return faceType;
	}
}
