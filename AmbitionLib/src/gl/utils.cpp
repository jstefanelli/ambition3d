#include <ambition/gl/utils.h>

namespace ambition::gl {
	bool CheckGL(std::string_view file, int line) {
		GLenum err = GL_NO_ERROR;
		bool any_error = false;
		do {
			err = glGetError();
			if (err != GL_NO_ERROR) {
				any_error = true;

				switch (err) {
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
		} while (err != GL_NO_ERROR);

#ifdef AMBITIONLIB_GL_STRICT
		if (any_error) {
			throw std::runtime_error("OpenGL Error(s) caught by AMBITIONLIB_GL_STRICT");
		}
#endif

		return !any_error;
	}

	GLenum utils::ToInternalFormat(resources::PixelFormat fmt, resources::PixelType type)
	{
		switch (fmt) {
		case resources::PixelFormat::BGR:
		case resources::PixelFormat::RGB:
			switch (type) {
			case resources::PixelType::BYTE:
				return GL_RGB8;
			case resources::PixelType::FLOAT:
				return GL_RGB32F;
			case resources::PixelType::INT:
				return GL_RGB32UI;
			case resources::PixelType::SHORT:
				return GL_RGB16;
			default:
				return GL_RGBA;
			}
		case resources::PixelFormat::BGRA:
		case resources::PixelFormat::RGBA:
			switch (type) {
			case resources::PixelType::BYTE:
				return GL_RGBA8;
			case resources::PixelType::FLOAT:
				return GL_RGBA32F;
			case resources::PixelType::INT:
				return GL_RGBA32UI;
			case resources::PixelType::SHORT:
				return GL_RGBA16;
			default:
				return GL_RGBA;
			}
		case resources::PixelFormat::DEPTH:
			switch (type) {
			case resources::PixelType::BYTE:
			case resources::PixelType::SHORT:
				return GL_DEPTH_COMPONENT16;
			case resources::PixelType::FLOAT:
			case resources::PixelType::INT:
				return GL_DEPTH_COMPONENT32F;
			default:
				return GL_DEPTH_COMPONENT;
			}
		case resources::PixelFormat::RED:
		case resources::PixelFormat::LUMINANCE:
			switch (type) {
			case resources::PixelType::BYTE:
				return GL_R8;
			case resources::PixelType::SHORT:
				return GL_R16;
			case resources::PixelType::FLOAT:
				return GL_R32F;
			case resources::PixelType::INT:
				return GL_R32I;
			default:
				return GL_R;
			}
		default:
			return GL_RGBA;
		}
	}

	GLenum utils::ToFormat(resources::PixelFormat fmt)
	{
		switch (fmt) {
		case resources::PixelFormat::BGR:
			return GL_BGR;
		case resources::PixelFormat::BGRA:
			return GL_BGRA;
		case resources::PixelFormat::DEPTH:
			return GL_DEPTH_COMPONENT;
		case resources::PixelFormat::LUMINANCE:
		case resources::PixelFormat::RED:
			return GL_RED;
		case resources::PixelFormat::RGB:
			return GL_RGB;
		case resources::PixelFormat::RGBA:
			return GL_RGBA;
		default:
			return GL_RGBA;
		}
	}

	GLenum utils::ToType(resources::PixelType type)
	{
		switch (type) {
		case resources::PixelType::BYTE:
			return GL_UNSIGNED_BYTE;
		case resources::PixelType::FLOAT:
			return GL_FLOAT;
		case resources::PixelType::INT:
			return GL_UNSIGNED_INT;
		case resources::PixelType::SHORT:
			return GL_UNSIGNED_SHORT;
		default:
			return GL_BYTE;
		}
	}

}