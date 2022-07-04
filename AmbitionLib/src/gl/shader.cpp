#include "ambition/gl/shader.h"
#include "ambition/gl/utils.h"
#include <iostream>

namespace ambition::gl {
	bool Shader::LoadSync(const std::string& vsSource, const std::string& fsSource)
	{
		int shaders[2];
		shaders[0] = glCreateShader(GL_VERTEX_SHADER);
		CHECK_GL();

		int size = (int)vsSource.length();
		auto source = vsSource.c_str();
		glShaderSource(shaders[0], 1, &source, &size);
		CHECK_GL();

		glCompileShader(shaders[0]);
		CHECK_GL();

		int param;
		glGetShaderiv(shaders[0], GL_INFO_LOG_LENGTH, &param);
		CHECK_GL();
		if (param > 0) {
			std::vector<char> infoLog(param, 0);
			glGetShaderInfoLog(shaders[0], (GLsizei)infoLog.size(), nullptr, &infoLog[0]);
			CHECK_GL();

			std::cout << "Vertex shader: " << std::endl << std::string(infoLog.begin(), infoLog.end()) << std::endl;
		}

		glGetShaderiv(shaders[0], GL_COMPILE_STATUS, &param);
		if (param != GL_TRUE) {
			return false;
		}

		size = (int)fsSource.length();
		source = fsSource.c_str();

		shaders[1] = glCreateShader(GL_FRAGMENT_SHADER);
		CHECK_GL();

		glShaderSource(shaders[1], 1, &source, &size);
		CHECK_GL();

		glCompileShader(shaders[1]);
		CHECK_GL();

		glGetShaderiv(shaders[1], GL_INFO_LOG_LENGTH, &param);
		CHECK_GL();
		if (param > 0) {
			std::vector<char> infoLog(param, 0);
			glGetShaderInfoLog(shaders[1], (GLsizei)infoLog.size(), nullptr, &infoLog[0]);
			CHECK_GL();

			std::cout << "Fragment shader: " << std::endl << std::string(infoLog.begin(), infoLog.end()) << std::endl;
		}

		glGetShaderiv(shaders[1], GL_COMPILE_STATUS, &param);
		CHECK_GL();

		if (param != GL_TRUE) {
			return false;
		}

		id = glCreateProgram();
		CHECK_GL();

		glAttachShader(id, shaders[0]);
		CHECK_GL();
		glAttachShader(id, shaders[1]);
		CHECK_GL();

		glLinkProgram(id);
		CHECK_GL();

		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &param);
		CHECK_GL();

		if (param > 0) {
			std::vector<char> infoLog(param, 0);
			glGetProgramInfoLog(id, (GLsizei)infoLog.size(), nullptr, &infoLog[0]);
			CHECK_GL();

			std::cout << "Program: " << std::endl << std::string(infoLog.begin(), infoLog.end()) << std::endl;
		}

		glDeleteShader(shaders[0]);
		CHECK_GL();
		glDeleteShader(shaders[1]);
		CHECK_GL();

		allocated = true;
		return true;
	}
	RenderTask_t<bool> Shader::Load(const std::string& vsSource, const std::string& fsSource)
	{
		co_return LoadSync(vsSource, fsSource);
	}

	bool Shader::DeallocateSync()
	{
		if (!allocated) {
			return true;
		}

		glDeleteProgram(id);
		allocated = false;

		return true;
	}
	
	RenderTask_t<bool> Shader::Deallocate()
	{
		co_return DeallocateSync();
	}

	RenderTask_t<bool> Shader::Deallocate(GLint id)
	{
		if (glIsProgram(id)) {
			glDeleteProgram(id);
			CHECK_GL();
		}
		CHECK_GL();
		co_return true;
	}

	Shader::~Shader()
	{
		if (allocated) {
			Shader::Deallocate(id);
		}
	}
}