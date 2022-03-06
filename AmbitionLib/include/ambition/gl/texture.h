#pragma once
#include <glm/glm.hpp>
#include <gl/glew.h>
#include <memory>
#include <vector>
#include "../renderTask.h"

namespace ambition::gl {
    class Texture {
    protected:
        GLuint id;
        glm::ivec2 size;
        bool allocated;
        int max_mip_level;
		int min_mip_level;

        Texture();
		static RenderTask_t<bool> Deallocate(GLuint id);
    public:
        static std::shared_ptr<Texture> Generate();

        inline bool Allocated() const {
			return allocated;
		}
        inline GLuint Id() const {
			return id;
		}
        inline int MinMipLevel() const {
			return min_mip_level;
		}

		bool AllocateSync(GLenum target_format, glm::ivec2, int maxMipLevel);
		bool LoadSync(std::shared_ptr<std::vector<unsigned char>> data, GLenum src_format, int level = 0, bool generateMips = true);
		bool DeallocateSync();
        RenderTask_t<bool> Allocate(GLenum target_format, glm::ivec2 size, int maxMipLevel);
        RenderTask_t<bool> Load(std::shared_ptr<std::vector<unsigned char>> data, GLenum src_format, int level = 0, bool generateMips = true);

        RenderTask_t<bool> Deallocate();

		~Texture();
    };
}