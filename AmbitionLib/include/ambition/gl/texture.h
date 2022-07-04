#pragma once
#include <glm/glm.hpp>
#include <gl/glew.h>
#include <memory>
#include <vector>
#include "../renderTask.h"
#include "../resources/texture.h"

namespace ambition::gl {
	template<int dims>
	class MipInfo {
	protected:
		std::vector<bool> levels;
		unsigned int max = 0;
		unsigned int min = 0;

		void Update();
	public:
		void Init(glm::vec<dims, int, glm::defaultp> size);
		void EnableLevel(unsigned int lvl);
		void DisableLevel(unsigned int lvl);
		void EnableAutoMips();

		bool IsLevelEnabled(unsigned int lvl) const;
		unsigned int MaxLevel() const;
		unsigned int MinLevel() const;

		int TotalLevels() const;

		void Apply(GLenum target) const;
	};

	template<int dims>
    class Texture {
    protected:
        GLuint id;
        bool allocated;
		MipInfo<dims> mip;
		glm::vec<dims, int, glm::defaultp> size;

        Texture();
		static RenderTask_t<bool> Deallocate(GLuint id);
    public:

        inline bool Allocated() const {
			return allocated;
		}
        inline GLuint Id() const {
			return id;
		}
		inline glm::vec<dims, int, glm::defaultp> Size() const {
			return size;
		}

		virtual bool DeallocateSync();
        RenderTask_t<bool> Deallocate();

		const MipInfo<dims>& Mip() const;

		virtual ~Texture();
    };

	class Texture2D : public Texture<2> {
	protected:
		glm::mat3 base_uv_matrix;
		bool LoadLevel(resources::PixelFormat format, resources::PixelType type, glm::ivec2 size, int level, const unsigned char* data, bool generateMipMaps, bool applyLevel);
	public:
		Texture2D();

		bool LoadSync(std::shared_ptr<resources::Texture2D> texture, int level, bool generateMipMaps = true);
		RenderTask_t<bool> Load(std::shared_ptr<resources::Texture2D> texture, int level, bool generateMipMaps = true);

		bool AllocateSync(glm::ivec2 size, resources::PixelFormat format, resources::PixelType type, bool immediatelyAvailable = false);
		RenderTask_t<bool> Allocate(glm::ivec2 size, resources::PixelFormat format, resources::PixelType type, bool immediatelyAvailable = false);

		bool DeallocateSync() override;

		inline glm::mat3 BaseUVMatrix() const {
			return base_uv_matrix;
		}

		~Texture2D() override = default;
	};
}