#include <ambition/gl/texture.h>
#include <ambition/gl/utils.h>

namespace ambition::gl {

	Texture::Texture() : id(0), size(0, 0), allocated(false), max_mip_level(0), min_mip_level(0) {

	}

	std::shared_ptr<Texture> Texture::Generate() {
		return std::shared_ptr<Texture>(new Texture());
	}

	bool Texture::AllocateSync(GLenum target_format, glm::ivec2 target_size, int maxMipLevel) {
		if (allocated) {
			return false;
		}

		glGenTextures(1, &id);
		CHECK_GL();
		glBindTexture(GL_TEXTURE_2D, id);
		CHECK_GL();
		glTexImage2D(GL_TEXTURE_2D, 0, target_format, target_size.x, target_size.y, 0, target_format, GL_UNSIGNED_BYTE, nullptr);
		CHECK_GL();
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, (float) maxMipLevel);
		CHECK_GL();
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, (float) maxMipLevel);
		CHECK_GL();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		CHECK_GL();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, maxMipLevel == 0 ? GL_LINEAR : GL_LINEAR_MIPMAP_LINEAR);
		CHECK_GL();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, POT_N(target_size.x) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
		CHECK_GL();
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, POT_N(target_size.y) ? GL_REPEAT : GL_CLAMP_TO_EDGE);
		CHECK_GL();

		size = target_size;
		max_mip_level = maxMipLevel;
		min_mip_level = maxMipLevel;
		allocated = true;

		return true;
	}

	bool
	Texture::LoadSync(std::shared_ptr<std::vector<unsigned char>> data, GLenum src_format, int level, bool generateMips) {
		if (!allocated) {
			return false;
		}

		if (level > max_mip_level) {
			std::cerr << "[GL] Failed to load texture MIP level " << level << " because it's higher than the provided maxMipLevel" << std::endl;
			return false;
		}

		if (level > 0 && level < min_mip_level - 1) {
			std::cerr << "[GL] Textures must be loaded from less detailed MIP level to most detailed, or load the highest detailed level and set 'generateMips' to true" << std::endl;
			return false;
		}

		glBindTexture(GL_TEXTURE_2D, id);
		CHECK_GL();
		glm::ivec2 mip_size = size;
		for (int i = 0; i < level; i++) {
			mip_size.x /= 2;
			mip_size.y /= 2;
		}

		glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, mip_size.x, mip_size.y, src_format, GL_UNSIGNED_BYTE, &(*data)[0]);
		CHECK_GL();

		if (level == 0 && generateMips) {
			glGenerateMipmap(GL_TEXTURE_2D);
			CHECK_GL();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, 0);
			CHECK_GL();
			min_mip_level = 0;
		} else {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, level);
			CHECK_GL();
			min_mip_level = level;
		}

		return true;
	}

	bool Texture::DeallocateSync() {
		if (!allocated)
			return false;

		glDeleteTextures(1, &id);
		CHECK_GL();

		allocated = false;
		size = glm::ivec2(0, 0);
		max_mip_level = 0;
		min_mip_level = 0;

		return true;
	}

	RenderTask_t<bool> Texture::Allocate(GLenum target_format, glm::ivec2 target_size, int maxMipLevel) {
		co_return AllocateSync(target_format, target_size, maxMipLevel);
	}

	RenderTask_t<bool> Texture::Load(std::shared_ptr<std::vector<unsigned char>> data, GLenum src_format, int level, bool generateMips) {
		co_return LoadSync(data, level, generateMips);
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
}