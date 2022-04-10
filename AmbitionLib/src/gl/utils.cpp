#include <ambition/gl/utils.h>

bool ambition::gl::CheckGL(std::string_view file, int line) {
	GLenum err = GL_NO_ERROR;
	bool any_error = false;
	do {
		err = glGetError();
		if (err != GL_NO_ERROR) {
			any_error = true;

			switch(err) {
				case GL_OUT_OF_MEMORY:
					std::cerr << "[GL] GL_OUT_OF_MEMORY @ " << file << ":" << line << std::endl;
					break;
				case GL_INVALID_ENUM:
					std::cerr << "[GL] GL_INVALID_ENUM @ " << file << ":" << line << std::endl;
					break;
				case GL_INVALID_VALUE:
					std::cerr << "[GL] GL_INVALID_VALUE @ " << file << ":" << line << std::endl;
					break;
				case GL_INVALID_OPERATION:
					std::cerr << "[GL] GL_INVALID_OPERATION @ " << file << ":" << line << std::endl;
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					std::cerr << "[GL] GL_INVALID_FRAMEBUFFER_OPERATION @ " << file << ":" << line << std::endl;
					break;
				case GL_STACK_OVERFLOW:
					std::cerr << "[GL] GL_STACK_OVERFLOW @ " << file << ":" << line << std::endl;
					break;
				case GL_STACK_UNDERFLOW:
					std::cerr << "[GL] GL_STACK_UNDERFLOW @ " << file << ":" << line << std::endl;
					break;
				default:
					std::cerr << "[GL] GL_UNKNOWN_ERROR: " << std::hex << err << std::dec << " @ " << file << ":" << line << std::endl;
					break;
			}
		}
	} while(err != GL_NO_ERROR);

#ifdef AMBITIONLIB_GL_STRICT
	if (any_error) {
		throw std::runtime_error("OpenGL Error(s) caught by AMBITIONLIB_GL_STRICT");
	}
#endif

	return !any_error;
}