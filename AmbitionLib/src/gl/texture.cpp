#include <ambition/gl/texture.h>
#include <ambition/gl/utils.h>

namespace ambition::gl {
	template<int dims>
	void MipInfo<dims>::Update()
	{
		int lvl = 0;
		auto it = levels.begin();
		while (!*it) {
			it++;
			lvl++;
			if (it == levels.end())
				return;
		}
		min = lvl;
		it++;
		while (it != levels.end() && * it) {
			if (it == levels.end())
				break;
			lvl++;
			it++;
		}
		max = lvl;
	}

	template<int dims>
	inline void MipInfo<dims>::Init(glm::vec<dims, int, glm::defaultp> size)
	{
		levels.clear();
		levels.push_back(false);
		while (size != glm::vec<dims, int, glm::defaultp>(1)) {
			size = glm::max(size / 2, glm::vec<dims, int, glm::defaultp>(1));
			levels.push_back(false);
		}
		Update();
	}

	template<int dims>
	inline void MipInfo<dims>::EnableLevel(unsigned int lvl)
	{
		if (lvl >= levels.size())
			return;

		levels[lvl] = true;
		Update();
	}

	template<int dims>
	inline void MipInfo<dims>::DisableLevel(unsigned int lvl)
	{
		if (lvl >= levels.size())
			return;
		levels[lvl] = false;
		Update();
	}

	template<int dims>
	inline void MipInfo<dims>::EnableAutoMips()
	{
		for (auto it = levels.begin(); it != levels.end(); it++) {
			*it = true;
		}
		Update();
	}

	template<int dims>
	bool MipInfo<dims>::IsLevelEnabled(unsigned int lvl) const
	{
		if (lvl >= levels.size())
			return false;
		return levels[lvl];
	}

	template<int dims>
	unsigned int MipInfo<dims>::MaxLevel() const
	{
		return max;
	}

	template<int dims>
	unsigned int MipInfo<dims>::MinLevel() const
	{
		return min;
	}

	template<int dims>
	inline int MipInfo<dims>::TotalLevels() const
	{
		return levels.size();
	}

	template<int dims>
	inline void MipInfo<dims>::Apply(GLenum target) const
	{
		glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, min);
		CHECK_GL();
		glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, max);
		CHECK_GL();
	}

	template<int dims>
	Texture<dims>::Texture() : id(0), allocated(false), size(0) {

	}

	template<int dims>
	bool Texture<dims>::DeallocateSync() {
		if (!allocated)
			return false;

		glDeleteTextures(1, &id);
		size = glm::vec<dims, int, glm::defaultp>(0);
		CHECK_GL();

		allocated = false;

		return true;
	}

	template<int dims>
	RenderTask_t<bool> Texture<dims>::Deallocate() {
		co_return DeallocateSync();
	}

	template<int dims>
	const MipInfo<dims>& Texture<dims>::Mip() const
	{
		return mip;
	}

	template<int dims>
	RenderTask_t<bool> Texture<dims>::Deallocate(GLuint id) {
		glDeleteTextures(1, &id);
		CHECK_GL();

		co_return true;
	}

	template<int dims>
	Texture<dims>::~Texture() {
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
		base_uv_matrix(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)) {
		size.x = 0;
		size.y = 0;
	}

	bool Texture2D::LoadLevel(resources::PixelFormat format, resources::PixelType type, glm::ivec2 size, int level, const unsigned char* data, bool generateMipMaps, bool applyLevel)
	{
		if (!allocated && level != 0)
			return false;

		if (!allocated) {
			glGenTextures(1, &id);
			CHECK_GL();
		}
		glBindTexture(GL_TEXTURE_2D, id);
		CHECK_GL();
		if (mip.IsLevelEnabled(level) || (allocated && level == 0)) {
			glTexSubImage2D(GL_TEXTURE_2D, level, 0, 0, size.x, size.y, utils::ToFormat(format), utils::ToType(type), data);
			CHECK_GL();
		} else {
			glTexImage2D(GL_TEXTURE_2D, level, utils::ToInternalFormat(format, type), size.x, size.y, 0, utils::ToFormat(format), utils::ToType(type), data);
			CHECK_GL();
		}

		if (level == 0 && !mip.IsLevelEnabled(level)) {
			this->size = size;
			mip.Init(size);
			auto pot_w = POT_N(size.x);
			auto pot_h = POT_N(size.y);
			if (pot_w)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			CHECK_GL();

			if (pot_h)
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			else
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			CHECK_GL();

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			CHECK_GL();
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			CHECK_GL();

			if (generateMipMaps) {
				glGenerateMipmap(GL_TEXTURE_2D);
				CHECK_GL();
				mip.EnableAutoMips();
			}

			allocated = true;
		}
		if (applyLevel) {
			mip.EnableLevel(level);
			mip.Apply(GL_TEXTURE_2D);
		}

		return true;
	}

	bool Texture2D::LoadSync(std::shared_ptr<resources::Texture2D> texture, int level, bool generateMipMaps) {
		return LoadLevel(texture->PixelFormat(), texture->PixelType(), texture->Size(), level, &texture->Data()[0], true, true);
	}

	RenderTask_t<bool> Texture2D::Load(std::shared_ptr<resources::Texture2D> texture, int level, bool generateMipMaps)
	{
		co_return LoadSync(std::move(texture), level, generateMipMaps);
	}

	bool Texture2D::AllocateSync(glm::ivec2 size, resources::PixelFormat format, resources::PixelType type, bool immediatelyAvailable)
	{
		return LoadLevel(format, type, size, 0, nullptr, false, immediatelyAvailable);
	}

	RenderTask_t<bool> Texture2D::Allocate(glm::ivec2 size, resources::PixelFormat format, resources::PixelType type, bool immediatelyAvailable)
	{
		co_return AllocateSync(size, format, type, immediatelyAvailable);
	}
}