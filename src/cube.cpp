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

Cube::Cube(int topTexture, int sideTexture, int bottomTexture)
	: BasicMesh{
		{
			// top face
			{{0.f, 1.f, 1.f}, textureIndexToCoords(topTexture, Top, Left)},
			{{1.f, 1.f, 1.f}, textureIndexToCoords(topTexture, Top, Right)},
			{{0.f, 1.f, 0.f}, textureIndexToCoords(topTexture, Bottom, Left)},
			{{1.f, 1.f, 0.f}, textureIndexToCoords(topTexture, Bottom, Right)},

			// front face
			{{0.f, 1.f, 0.f}, textureIndexToCoords(sideTexture, Top, Left)},
			{{1.f, 1.f, 0.f}, textureIndexToCoords(sideTexture, Top, Right)},
			{{0.f, 0.f, 0.f}, textureIndexToCoords(sideTexture, Bottom, Left)},
			{{1.f, 0.f, 0.f}, textureIndexToCoords(sideTexture, Bottom, Right)},

			// left face
			{{0.f, 1.f, 1.f}, textureIndexToCoords(sideTexture, Top, Left)},
			{{0.f, 1.f, 0.f}, textureIndexToCoords(sideTexture, Top, Right)},
			{{0.f, 0.f, 1.f}, textureIndexToCoords(sideTexture, Bottom, Left)},
			{{0.f, 0.f, 0.f}, textureIndexToCoords(sideTexture, Bottom, Right)},

			// back face
			{{1.f, 1.f, 1.f}, textureIndexToCoords(sideTexture, Top, Left)},
			{{0.f, 1.f, 1.f}, textureIndexToCoords(sideTexture, Top, Right)},
			{{1.f, 0.f, 1.f}, textureIndexToCoords(sideTexture, Bottom, Left)},
			{{0.f, 0.f, 1.f}, textureIndexToCoords(sideTexture, Bottom, Right)},

			// right face
			{{1.f, 1.f, 0.f}, textureIndexToCoords(sideTexture, Top, Left)},
			{{1.f, 1.f, 1.f}, textureIndexToCoords(sideTexture, Top, Right)},
			{{1.f, 0.f, 0.f}, textureIndexToCoords(sideTexture, Bottom, Left)},
			{{1.f, 0.f, 1.f}, textureIndexToCoords(sideTexture, Bottom, Right)},

			// bottom face
			{{1.f, 0.f, 1.f}, textureIndexToCoords(bottomTexture, Top, Left)},
			{{0.f, 0.f, 1.f}, textureIndexToCoords(bottomTexture, Top, Right)},
			{{1.f, 0.f, 0.f}, textureIndexToCoords(bottomTexture, Bottom, Left)},
			{{0.f, 0.f, 0.f}, textureIndexToCoords(bottomTexture, Bottom, Right)},
		},
		{
			// top face
			0, 1, 2,
			1, 3, 2,

			// front face
			4, 5, 6,
			5, 7, 6,

			// left face
			8, 9, 10,
			9, 11, 10,

			// back face
			12, 13, 14,
			13, 15, 14,

			// right face
			16, 17, 18,
			17, 19, 18,

			// bottom face
			20, 21, 22,
			21, 23, 22,
		}
	}
{}
