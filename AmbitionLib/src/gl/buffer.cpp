#include <ambition/gl/buffer.h>
#include <ambition/gl/utils.h>

namespace ambition::gl {
	template<GLenum target>
	std::shared_ptr<Buffer<target>> Buffer<target>::Generate() {
		return std::shared_ptr<Buffer>();
	}

	template<GLenum target>
	template<typename T>
	RenderTask_t<bool> Buffer<target>::Load(std::shared_ptr<std::vector<T>> data) {
		if (allocated) {
			co_return false;
		}

		glGenBuffers(1, &id);
		CHECK_GL();
		glBindBuffer(target, id);
		CHECK_GL();
		size_t targetSize = data->size() * sizeof(T);
		glBufferData(target, targetSize, &(*data)[0], GL_STATIC_DRAW);
		CHECK_GL();

		size = targetSize;
		allocated = true;

		co_return true;
	}

	template<GLenum target>
	template<typename T>
	RenderTask_t<bool> Buffer<target>::Replace(std::shared_ptr<std::vector<T>> data, int start) {
		if (!allocated) {
			co_return false;
		}

		size_t startOffset = start * sizeof(T);
		size_t dataSize = data->size() * sizeof(T);

		if (startOffset + dataSize >= size) {
			co_return false;
		}

		glBindBuffer(target, id);
		CHECK_GL();
		glBufferSubData(target, startOffset, dataSize, &(*data)[0]);
		CHECK_GL();

		co_return true;
	}

	template<GLenum target>
	RenderTask_t<bool> Buffer<target>::Allocate(size_t bytes) {
		if (allocated) {
			co_return false;
		}

		glGenBuffers(1, &id);
		CHECK_GL();
		glBufferData(target, bytes, nullptr);
		CHECK_GL();

		size = bytes;
		allocated = true;

		co_return true;
	}

	template<GLenum target>
	RenderTask_t<bool> Buffer<target>::Deallocate() {
		if(!allocated)
			co_return false;

		glDeleteBuffers(1, &id);
		CHECK_GL();

		allocated = false;
		size = 0;
		id = 0;

		co_return true;
	}

	template<GLenum target>
	RenderTask_t<bool> Buffer<target>::Deallocate(GLuint id) {
		glDeleteBuffers(1, &id);
		CHECK_GL();
	}

	template<GLenum target>
	Buffer<target>::~Buffer() {
		if (allocated) {
			Deallocate(id);
		}
	}

	template<GLenum target>
	RenderTask_t<bool> Buffer<target>::Bind() const {
		if (!allocated)
			co_return false;

		glBindBuffer(target, id);
		CHECK_GL();

		co_return true;
	}
}