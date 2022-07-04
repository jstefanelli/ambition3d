#pragma once
#include <gl/glew.h>
#include <string_view>
#include <iostream>
#include <exception>
#include "../resources/texture.h"

namespace ambition::gl {
	bool CheckGL(std::string_view file, int line);

	namespace utils {
		GLenum ToInternalFormat(resources::PixelFormat fmt, resources::PixelType type);
		GLenum ToFormat(resources::PixelFormat fmt);
		GLenum ToType(resources::PixelType type);
	}
}

#define CHECK_GL() ambition::gl::CheckGL(__FILE__, __LINE__)
#define POT_N(n) ((n & (n - 1)) == 0)