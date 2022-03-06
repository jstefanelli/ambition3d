#pragma once
#include <memory>
#include <gl/glew.h>
#include "../renderTask.h"

namespace ambition::gl {
	template<GLenum target>
	class Buffer {
	protected:
		size_t size;
		GLuint id;
		bool allocated;

		Buffer() = default;
		static RenderTask_t<bool> Deallocate(GLuint id);
	public:
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		static std::shared_ptr<Buffer> Generate();

		inline GLuint Id() const {
			return id;
		}

		inline size_t Size() const {
			return size;
		}

		inline bool Allocated() const {
			return allocated;
		}

		template<typename T>
		RenderTask_t<bool> Load(std::shared_ptr<std::vector<T>> data);
		template<typename T>
		RenderTask_t<bool> Replace(std::shared_ptr<std::vector<T>> data, int start);
		RenderTask_t<bool> Allocate(size_t bytes);
		RenderTask_t<bool> Deallocate();

		RenderTask_t<bool> Bind() const;

		~Buffer();
	};

}