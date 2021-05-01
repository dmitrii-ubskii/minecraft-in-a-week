#ifndef SRC_CUBE_H_
#define SRC_CUBE_H_

#include <array>

#include <glm/glm.hpp>

#include "opengl/basic_mesh.h"

class Cube
{
public:
	Cube(int topTexture_, int sideTexture_, int bottomTexture_)
		: topTexture{topTexture_}
		, sideTexture{sideTexture_}
		, bottomTexture{bottomTexture_}
	{}

	Cube(Cube&&) = default;
	Cube& operator=(Cube&&) = default;

	std::array<Vertex, 4> getTopFace(glm::vec3 position);
	std::array<Vertex, 4> getFrontFace(glm::vec3 position);
	std::array<Vertex, 4> getLeftFace(glm::vec3 position);
	std::array<Vertex, 4> getBackFace(glm::vec3 position);
	std::array<Vertex, 4> getRightFace(glm::vec3 position);
	std::array<Vertex, 4> getBottomFace(glm::vec3 position);

private:
	int topTexture;
	int sideTexture;
	int bottomTexture;
};

#endif // SRC_CUBE_H_
