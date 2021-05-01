#ifndef SRC_CHUNK_H_
#define SRC_CHUNK_H_

#include <array>
#include <cassert>

#include <glm/gtx/transform.hpp>

#include <DU/math.h>

#include "opengl/shader.h"

#include "cube.h"
#include "seeded_noise.h"

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
		static auto cube = Cube{0, 1, 2};

		for (auto bx = 0; bx < ChunkWidth; bx++)
		{
			for (auto bz = 0; bz < ChunkDepth; bz++)
			{
				for (auto by = 0; by < ChunkHeight; by++)
				{
					if (blocks[localCoordsToIndex({bx, by, bz})] == BlockType::Air)
					{
						break;
					}
					auto pos = glm::vec3{
						bx + chunkCoords.x * ChunkWidth,
						by + chunkCoords.y * ChunkHeight,
						bz + chunkCoords.z * ChunkDepth,
					};
					shader.setMat4("model", glm::translate(glm::mat4{1.f}, pos));
					cube.draw();
				}
			}
		}
	}

	static constexpr int ChunkWidth = 16;  // x
	static constexpr int ChunkHeight = 256;  // y
	static constexpr int ChunkDepth = 16;  // z

private:
	Coords chunkCoords;

	std::size_t localCoordsToIndex(Coords localCoords)
	{
		assert(localCoords.x < ChunkWidth);
		assert(localCoords.y < ChunkHeight);
		assert(localCoords.z < ChunkDepth);
		return (std::size_t)((localCoords.x * ChunkHeight + localCoords.y) * ChunkDepth + localCoords.z);
	}

	std::array<BlockType, ChunkWidth * ChunkHeight * ChunkDepth> blocks;
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
