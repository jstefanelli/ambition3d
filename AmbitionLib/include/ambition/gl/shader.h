#pragma once
#include <gl/glew.h>
#include <string>
#include "../renderTask.h"

namespace ambition::gl {
	class Shader {
	protected:
		bool allocated;
		GLint id;
	public:
		bool LoadSync(const std::string& vsSource, const std::string& fsSource);
		RenderTask_t<bool> Load(const std::string& vsSource, const std::string& fsSource);

		bool DeallocateSync();
		RenderTask_t<bool> Deallocate();

		static RenderTask_t<bool> Deallocate(GLint id);

		~Shader();
		inline GLint Id() const {
			return id;
		}
	};
}