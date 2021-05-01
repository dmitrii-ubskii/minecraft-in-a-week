#include "cube.h"

enum HAlign
{
	Left, Right
};

enum VAlign
{
	Top, Bottom
};

constexpr Vertex::TextureCoords textureIndexToCoords(int id, VAlign v, HAlign h)
{
	auto x = (float)(h + id % 8);
	auto y = (float)(v + id / 8);
	return {x / 8.f, y / 8.f};
}

std::array<Vertex, 4> Cube::getTopFace(glm::vec3 position)
{
	return std::array<Vertex, 4>{{
		{{position.x + 0.f, position.y + 1.f, position.z + 1.f}, textureIndexToCoords(topTexture, Top, Left), {0.f, 1.f, 0.f}},
		{{position.x + 1.f, position.y + 1.f, position.z + 1.f}, textureIndexToCoords(topTexture, Top, Right), {0.f, 1.f, 0.f}},
		{{position.x + 0.f, position.y + 1.f, position.z + 0.f}, textureIndexToCoords(topTexture, Bottom, Left), {0.f, 1.f, 0.f}},
		{{position.x + 1.f, position.y + 1.f, position.z + 0.f}, textureIndexToCoords(topTexture, Bottom, Right), {0.f, 1.f, 0.f}},
	}};
}

std::array<Vertex, 4> Cube::getFrontFace(glm::vec3 position)
{
	return std::array<Vertex, 4>{{
		{{position.x + 0.f, position.y + 1.f, position.z + 0.f}, textureIndexToCoords(sideTexture, Top, Left), {0.f, 0.f, -1.f}},
		{{position.x + 1.f, position.y + 1.f, position.z + 0.f}, textureIndexToCoords(sideTexture, Top, Right), {0.f, 0.f, -1.f}},
		{{position.x + 0.f, position.y + 0.f, position.z + 0.f}, textureIndexToCoords(sideTexture, Bottom, Left), {0.f, 0.f, -1.f}},
		{{position.x + 1.f, position.y + 0.f, position.z + 0.f}, textureIndexToCoords(sideTexture, Bottom, Right), {0.f, 0.f, -1.f}},
	}};
}

std::array<Vertex, 4> Cube::getLeftFace(glm::vec3 position)
{
	return std::array<Vertex, 4>{{
		{{position.x + 0.f, position.y + 1.f, position.z + 1.f}, textureIndexToCoords(sideTexture, Top, Left), {-1.f, 0.f, 0.f}},
		{{position.x + 0.f, position.y + 1.f, position.z + 0.f}, textureIndexToCoords(sideTexture, Top, Right), {-1.f, 0.f, 0.f}},
		{{position.x + 0.f, position.y + 0.f, position.z + 1.f}, textureIndexToCoords(sideTexture, Bottom, Left), {-1.f, 0.f, 0.f}},
		{{position.x + 0.f, position.y + 0.f, position.z + 0.f}, textureIndexToCoords(sideTexture, Bottom, Right), {-1.f, 0.f, 0.f}},
	}};
}

std::array<Vertex, 4> Cube::getBackFace(glm::vec3 position)
{
	return std::array<Vertex, 4>{{
		{{position.x + 1.f, position.y + 1.f, position.z + 1.f}, textureIndexToCoords(sideTexture, Top, Left), {0.f, 0.f, 1.f}},
		{{position.x + 0.f, position.y + 1.f, position.z + 1.f}, textureIndexToCoords(sideTexture, Top, Right), {0.f, 0.f, 1.f}},
		{{position.x + 1.f, position.y + 0.f, position.z + 1.f}, textureIndexToCoords(sideTexture, Bottom, Left), {0.f, 0.f, 1.f}},
		{{position.x + 0.f, position.y + 0.f, position.z + 1.f}, textureIndexToCoords(sideTexture, Bottom, Right), {0.f, 0.f, 1.f}},
	}};
}

std::array<Vertex, 4> Cube::getRightFace(glm::vec3 position)
{
	return std::array<Vertex, 4>{{
		{{position.x + 1.f, position.y + 1.f, position.z + 0.f}, textureIndexToCoords(sideTexture, Top, Left), {1.f, 0.f, 0.f}},
		{{position.x + 1.f, position.y + 1.f, position.z + 1.f}, textureIndexToCoords(sideTexture, Top, Right), {1.f, 0.f, 0.f}},
		{{position.x + 1.f, position.y + 0.f, position.z + 0.f}, textureIndexToCoords(sideTexture, Bottom, Left), {1.f, 0.f, 0.f}},
		{{position.x + 1.f, position.y + 0.f, position.z + 1.f}, textureIndexToCoords(sideTexture, Bottom, Right), {1.f, 0.f, 0.f}},
	}};
}

std::array<Vertex, 4> Cube::getBottomFace(glm::vec3 position)
{
	return std::array<Vertex, 4>{{
		{{position.x + 1.f, position.y + 0.f, position.z + 1.f}, textureIndexToCoords(bottomTexture, Top, Left), {0.f, -1.f, -1.f}},
		{{position.x + 0.f, position.y + 0.f, position.z + 1.f}, textureIndexToCoords(bottomTexture, Top, Right), {0.f, -1.f, -1.f}},
		{{position.x + 1.f, position.y + 0.f, position.z + 0.f}, textureIndexToCoords(bottomTexture, Bottom, Left), {0.f, -1.f, -1.f}},
		{{position.x + 0.f, position.y + 0.f, position.z + 0.f}, textureIndexToCoords(bottomTexture, Bottom, Right), {0.f, -1.f, -1.f}},
	}};
}
