#include <ambition/gl/texture.h>
#include <ambition/gl/utils.h>

namespace ambition::gl {
	Texture::Texture() : id(0), allocated(false), max_mip_level(0), min_mip_level(0) {

	}

	bool Texture::DeallocateSync() {
		if (!allocated)
			return false;

		glDeleteTextures(1, &id);
		CHECK_GL();

		allocated = false;
		max_mip_level = 0;
		min_mip_level = 0;

		return true;
	}

	RenderTask_t<bool> Texture::Deallocate() {
		co_return DeallocateSync();
	}

	RenderTask_t<bool> Texture::Deallocate(GLuint id) {
		glDeleteTextures(1, &id);
		CHECK_GL();

		co_return true;
	}

	Texture::~Texture() {
		if (allocated) {
			Deallocate(id);
		}
	}

	bool Texture2D::DeallocateSync() {
		if(Texture::DeallocateSync()) {
			size = glm::ivec2(0, 0);
			return true;
		}

		return false;
	}

	Texture2D::Texture2D() : Texture(),
		size(0, 0),
		base_uv_matrix(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) {

	}
}