#pragma once
#include <glm/glm.hpp>
#include <gl/glew.h>
#include <memory>
#include <vector>
#include "../renderTask.h"
#include "../resources/texture.h"

namespace ambition::gl {
    class Texture {
    protected:
        GLuint id;
        bool allocated;
        int max_mip_level;
		int min_mip_level;

        Texture();
		static RenderTask_t<bool> Deallocate(GLuint id);
    public:

        inline bool Allocated() const {
			return allocated;
		}
        inline GLuint Id() const {
			return id;
		}
        inline int MinMipLevel() const {
			return min_mip_level;
		}
		inline int MaxMipLevel() const {
			return max_mip_level;
		}

		virtual bool DeallocateSync();
        RenderTask_t<bool> Deallocate();

		virtual ~Texture();
    };

	class Texture2D : public Texture {
	protected:
		glm::ivec2 size;
		glm::mat3 base_uv_matrix;
	public:
		Texture2D();

		bool LoadSync(std::shared_ptr<resources::Texture2D> texture, int level, bool generateMipMaps = true);
		RenderTask_t<bool> Load(std::shared_ptr<resources::Texture2D> texture, int level, bool generateMipMaps = true);

		bool AllocateSync(glm::ivec2 size, resources::PixelFormat format, resources::PixelType type);
		RenderTask_t<bool> Allocate(glm::ivec2 size, resources::PixelFormat format, resources::PixelType type);

		bool DeallocateSync() override;

		inline glm::ivec2 Size() const {
			return size;
		}
		inline glm::mat3 BaseUVMatrix() const {
			return base_uv_matrix;
		}

		~Texture2D() override = default;
	};
}