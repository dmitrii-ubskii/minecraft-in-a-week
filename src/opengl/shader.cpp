#include "shader.h"

#include <fstream>
#include <iostream>
#include <string>

#include <glad/glad.h>

#include <glm/gtc/type_ptr.hpp>

std::string readFile(std::filesystem::path path)
{
	auto stream = std::ifstream(path);
	std::stringstream buffer;
	buffer << stream.rdbuf();
	return buffer.str();
}

unsigned int compileVertexShader(std::filesystem::path vertexShaderSourcePath)
{
	unsigned int vertexShader;
	auto vertexShaderSource = readFile(vertexShaderSourcePath);
	auto vertexShaderSourceCstr = vertexShaderSource.c_str();
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSourceCstr, nullptr);
	glCompileShader(vertexShader);

	int success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (not success)
	{
		glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << "\n";
	}
	return vertexShader;
}

unsigned int compileFragmentShader(std::filesystem::path fragmentShaderSourcePath)
{
	unsigned int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	auto fragmentShaderSource = readFile(fragmentShaderSourcePath);
	auto fragmentShaderSourceCstr = fragmentShaderSource.c_str();
	glShaderSource(fragmentShader, 1, &fragmentShaderSourceCstr, nullptr);
	glCompileShader(fragmentShader);

	int success;
	char infoLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (not success)
	{
		glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << "\n";
	}
	return fragmentShader;
}

unsigned int linkShaderProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
	unsigned int shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (not success)
	{
		glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
		std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << "\n";
	}
	return shaderProgram;
}

Shader::Shader(std::filesystem::path vertexPath, std::filesystem::path fragmentPath)
{
	auto vertexShader = compileVertexShader(vertexPath);
	auto fragmentShader = compileFragmentShader(fragmentPath);
	programId = linkShaderProgram(vertexShader, fragmentShader);
	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);
}

void Shader::use()
{
	glUseProgram(programId);
}

void Shader::setFloat(std::string const& name, float value)
{
	glUniform1f(glGetUniformLocation(programId, name.c_str()), value);
}

void Shader::setInt(std::string const& name, int value)
{
	glUniform1i(glGetUniformLocation(programId, name.c_str()), value);
}

void Shader::setVec2(std::string const& name, glm::vec2 const& data)
{
	glUniform2fv(glGetUniformLocation(programId, name.c_str()), 1, glm::value_ptr(data));
}

void Shader::setVec3(std::string const& name, glm::vec3 const& data)
{
	glUniform3fv(glGetUniformLocation(programId, name.c_str()), 1, glm::value_ptr(data));
}

void Shader::setVec3(std::string const& name, float const* data)
{
	glUniform3fv(glGetUniformLocation(programId, name.c_str()), 1, data);
}

void Shader::setVec3(std::string const& name, float x, float y, float z)
{
	glUniform3f(glGetUniformLocation(programId, name.c_str()), x, y, z);
}

void Shader::setVec4(std::string const& name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(programId, name.c_str()), x, y, z, w);
}

void Shader::setMat4(std::string const& name, glm::mat4 const& data)
{
	glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(data));
}

void Shader::setMat4(std::string const& name, float const* data)
{
	glUniformMatrix4fv(glGetUniformLocation(programId, name.c_str()), 1, GL_FALSE, data);
}

