#pragma once
#include <memory>
#include <gl/glew.h>
#include "../renderTask.h"
#include "utils.h"

namespace ambition::gl {
	template<GLenum target>
	class Buffer {
	protected:
		size_t size;
		GLuint id;
		bool allocated;

		Buffer() = default;
	public:
		Buffer(const Buffer&) = delete;
		Buffer& operator=(const Buffer&) = delete;

		static std::shared_ptr<Buffer<target>> Generate() {
			return std::shared_ptr<Buffer>(new Buffer());
		}

		template<typename T>
		RenderTask_t<bool> Load(const std::vector<T>& data) {
			if (allocated) {
				co_return false;
			}

			glGenBuffers(1, &id);
			CHECK_GL();
			glBindBuffer(target, id);
			CHECK_GL();
			size_t targetSize = data.size() * sizeof(T);
			glBufferData(target, targetSize, &data[0], GL_STATIC_DRAW);
			CHECK_GL();

			size = targetSize;
			allocated = true;

			co_return true;
		}

		template<typename T>
		RenderTask_t<bool> Replace(const std::vector<T>& data, int start) {
			if (!allocated) {
				co_return false;
			}

			size_t startOffset = start * sizeof(T);
			size_t dataSize = data.size() * sizeof(T);

			if (startOffset + dataSize >= size) {
				co_return false;
			}

			glBindBuffer(target, id);
			CHECK_GL();
			glBufferSubData(target, startOffset, dataSize, &data[0]);
			CHECK_GL();

			co_return true;
		}

		RenderTask_t<bool> Allocate(size_t bytes) {
			if (allocated) {
				co_return false;
			}

			glGenBuffers(1, &id);
			CHECK_GL();
			glBufferData(target, bytes, nullptr, GL_STATIC_DRAW);
			CHECK_GL();

			size = bytes;
			allocated = true;

			co_return true;
		}

		RenderTask_t<bool> Deallocate() {
			if (!allocated)
				co_return false;

			glDeleteBuffers(1, &id);
			CHECK_GL();

			allocated = false;
			size = 0;
			id = 0;

			co_return true;
		}

		static RenderTask_t<bool> Deallocate(GLuint id) {
			if (glIsBuffer(id) != GL_TRUE)
				co_return false;

			glDeleteBuffers(1, &id);
			co_return CHECK_GL();
		}

		~Buffer() {
			if (allocated) {
				Deallocate(id);
			}
		}

		RenderTask_t<bool> Bind() const {
			if (!allocated) {
				std::cout << "Buffer not binding." << std::endl;
				co_return false;
			}

			glBindBuffer(target, id);
			CHECK_GL();

			co_return true;
		}

		inline GLuint Id() const {
			return id;
		}

		inline size_t Size() const {
			return size;
		}

		inline bool Allocated() const {
			return allocated;
		}
	};

}