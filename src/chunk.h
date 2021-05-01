#ifndef SRC_CHUNK_H_
#define SRC_CHUNK_H_

#include <array>
#include <cassert>
#include <unordered_map>

#include <glm/gtx/transform.hpp>

#include <DU/math.h>

#include "opengl/shader.h"

#include "cube.h"
#include "seeded_noise.h"

#include <iostream>

enum class BlockType
{
	Air, Grass
};

struct Coords
{
	int x;
	int y;
	int z;
};

class Chunk
{
public:
	Chunk(int x, int z, Noise& device)
		: chunkCoords{x, 0, z}
	{
		cubes.insert({BlockType::Grass, Cube{0, 1, 2}});

		for (auto bx = 0; bx < ChunkWidth; bx++)
		{
			for (auto bz = 0; bz < ChunkDepth; bz++)
			{
				auto altitude = device.at({
					(float)(bx + chunkCoords.x * ChunkWidth) / 16.f,
					(float)(bz + chunkCoords.z * ChunkDepth) / 16.f
				});
				for (auto by = 0; by < altitude; by++)
					blocks[localCoordsToIndex({bx, by, bz})] = BlockType::Grass;
				for (auto by = altitude; by < ChunkHeight; by++)
					blocks[localCoordsToIndex({bx, by, bz})] = BlockType::Air;
			}
		}

		makeMesh();
	}

	Chunk(Chunk&&) = default;
	Chunk& operator=(Chunk&&) = default;

	BlockType& operator[](Coords globalCoords)
	{
		auto localCoords = Coords{
			.x = globalCoords.x - chunkCoords.x * ChunkWidth,
			.y = globalCoords.y - chunkCoords.y * ChunkHeight,
			.z = globalCoords.z - chunkCoords.z * ChunkDepth
		};
		return blocks[localCoordsToIndex(localCoords)];
	}

	void draw(Shader& shader)
	{
		auto pos = glm::vec3{
			chunkCoords.x * ChunkWidth,
			chunkCoords.y * ChunkHeight,
			chunkCoords.z * ChunkDepth,
		};
		shader.setMat4("model", glm::translate(glm::mat4{1.f}, pos));
		chunkMesh.draw();
	}

	static constexpr int ChunkWidth = 16;  // x
	static constexpr int ChunkHeight = 256;  // y
	static constexpr int ChunkDepth = 16;  // z

private:
	Coords chunkCoords;

	void makeMesh()
	{
		auto const xVertices = ChunkWidth + 1;
		auto const yVertices = ChunkHeight + 1;
		auto const zVertices = ChunkDepth + 1;
		auto const maxVertices = 2 * (xVertices * yVertices + yVertices * zVertices + zVertices * xVertices);

		auto vertices = std::vector<Vertex>{};
		vertices.reserve(maxVertices);

		for (auto x = 0; x < ChunkWidth; x++)
		{
			for (auto y = 0; y < ChunkHeight; y++)
			{
				for (auto z = 0; z < ChunkDepth; z++)
				{
					auto index = localCoordsToIndex({x, y, z});
					if (blocks[index] == BlockType::Air)
						continue;
					if (x == 0 || blocks[localCoordsToIndex({x - 1, y, z})] == BlockType::Air)
					{
						auto faceVertices = cubes.at(blocks[index]).getLeftFace({x, y, z});
						std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
					}
					if (x == ChunkWidth - 1 || blocks[localCoordsToIndex({x + 1, y, z})] == BlockType::Air)
					{
						auto faceVertices = cubes.at(blocks[index]).getRightFace({x, y, z});
						std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
					}

					if (y == 0 || blocks[localCoordsToIndex({x, y - 1, z})] == BlockType::Air)
					{
						auto faceVertices = cubes.at(blocks[index]).getBottomFace({x, y, z});
						std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
					}
					if (y == ChunkHeight - 1 || blocks[localCoordsToIndex({x, y + 1, z})] == BlockType::Air)
					{
						auto faceVertices = cubes.at(blocks[index]).getTopFace({x, y, z});
						std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
					}

					if (z == 0 || blocks[localCoordsToIndex({x, y, z - 1})] == BlockType::Air)
					{
						auto faceVertices = cubes.at(blocks[index]).getFrontFace({x, y, z});
						std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
					}
					if (z == ChunkDepth - 1 || blocks[localCoordsToIndex({x, y, z + 1})] == BlockType::Air)
					{
						auto faceVertices = cubes.at(blocks[index]).getBackFace({x, y, z});
						std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
					}
				}
			}
		}

		// bottom face
		for (auto x = 0; x < ChunkWidth; x++)
		{
			for (auto z = 0; z < ChunkDepth; z++)
			{
				auto index = localCoordsToIndex({x, 0, z});
				if (blocks[index] == BlockType::Air)
					continue;
				auto faceVertices = cubes.at(blocks[index]).getBottomFace({x, 0, z});
				std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
			}
		}

		auto indices = std::vector<unsigned>{};
		indices.reserve(vertices.size() * 6 / 4);  // 6 indices for each four vertices
		for (auto i = 0u; i < vertices.size(); i += 4)
		{
			indices.push_back(i);
			indices.push_back(i + 1);
			indices.push_back(i + 2);

			indices.push_back(i + 2);
			indices.push_back(i + 1);
			indices.push_back(i + 3);
		}
		chunkMesh = BasicMesh{vertices, indices};
	}

	BasicMesh chunkMesh;

	std::size_t localCoordsToIndex(Coords localCoords)
	{
		assert(localCoords.x < ChunkWidth);
		assert(localCoords.y < ChunkHeight);
		assert(localCoords.z < ChunkDepth);
		return (std::size_t)((localCoords.x * ChunkHeight + localCoords.y) * ChunkDepth + localCoords.z);
	}

	std::array<BlockType, ChunkWidth * ChunkHeight * ChunkDepth> blocks;
	std::unordered_map<BlockType, Cube> cubes;
};

constexpr Coords getBlockChunk(Coords globalCoords)
{
	return {
		.x = DU::floorDiv(globalCoords.x, Chunk::ChunkWidth),
		.y = DU::floorDiv(globalCoords.y, Chunk::ChunkHeight),
		.z = DU::floorDiv(globalCoords.z, Chunk::ChunkDepth)
	};
}

#endif // SRC_CHUNK_H_
