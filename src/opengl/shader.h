#ifndef SRC_OPENGL_SHADER_HPP_
#define SRC_OPENGL_SHADER_HPP_

#include <filesystem>

#include <glm/glm.hpp>

class Shader
{
public:
	Shader(std::filesystem::path vertexPath, std::filesystem::path fragmentPath);

	void use();

	void setFloat(std::string const& name, float value);
	void setInt(std::string const& name, int value);

	void setVec2(std::string const& name, glm::vec2 const& data);

	void setVec3(std::string const& name, glm::vec3 const& data);
	void setVec3(std::string const& name, float const* data);
	void setVec3(std::string const& name, float x, float y, float z);

	void setVec4(std::string const& name, float x, float y, float z, float w);

	void setMat4(std::string const& name, glm::mat4 const& data);
	void setMat4(std::string const& name, float const* data);

private:
	unsigned int programId;
};

#endif // SRC_OPENGL_SHADER_HPP_
