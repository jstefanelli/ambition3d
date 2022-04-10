#pragma once
#include <gl/glew.h>
#include <string_view>
#include <iostream>
#include <exception>

namespace ambition::gl {
	bool CheckGL(std::string_view file, int line);
}

#define CHECK_GL() ambition::gl::CheckGL(__FILE__, __LINE__)
#define POT_N(n) ((n & (n - 1)) == 0)