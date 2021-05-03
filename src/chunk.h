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
	Air, Grass, Dirt, Water
};
constexpr bool isOpaque(BlockType block)
{
	return block == BlockType::Dirt || block == BlockType::Grass;
}
constexpr bool isTranslucent(BlockType block)
{
	return block == BlockType::Water;
}

using Coords = glm::vec<3, int>;

class World;

class Chunk
{
public:
	Chunk(World* world_, int x, int z, Noise& device)
		: world{world_}
		, chunkCoords{x, 0, z}
	{
		cubes.insert({BlockType::Grass, Cube{0, 1, 2}});
		cubes.insert({BlockType::Dirt, Cube{2, 2, 2}});
		cubes.insert({BlockType::Water, Cube{3, 3, 3}});

		for (auto bx = 0; bx < ChunkWidth; bx++)
		{
			for (auto bz = 0; bz < ChunkDepth; bz++)
			{
				auto altitude = std::clamp(device.at({
					(float)(bx + chunkCoords.x * ChunkWidth) / 16.f,
					(float)(bz + chunkCoords.z * ChunkDepth) / 16.f
				}), 0, ChunkHeight - 1);
				
				for (auto by = 0; by < altitude - 1; by++)
				{
					blocks[localCoordsToIndex({bx, by, bz})] = BlockType::Dirt;
				}

				if (altitude >= WaterHeight)
				{
					blocks[localCoordsToIndex({bx, altitude - 1, bz})] = BlockType::Grass;
				}
				else
				{
					for (auto by = altitude - 1; by < WaterHeight; by++)
					{
						blocks[localCoordsToIndex({bx, by, bz})] = BlockType::Water;
					}
					altitude = WaterHeight;
				}

				for (auto by = altitude; by < ChunkHeight; by++)
				{
					blocks[localCoordsToIndex({bx, by, bz})] = BlockType::Air;
				}
			}
		}
	}

	Chunk(Chunk&&) = default;
	Chunk& operator=(Chunk&&) = default;

	BlockType& operator[](Coords globalCoords)
	{
		auto localCoords = globalCoords - chunkCoords * glm::vec<3, int>{ChunkWidth, ChunkHeight, ChunkDepth};
		return blocks[localCoordsToIndex(localCoords)];
	}

	void drawOpaque(Shader& shader)
	{
		auto pos = glm::vec3{
			chunkCoords.x * ChunkWidth,
			chunkCoords.y * ChunkHeight,
			chunkCoords.z * ChunkDepth,
		};
		shader.setMat4("model", glm::translate(glm::mat4{1.f}, pos));
		opaqueMesh.draw();
	}

	void drawTranslucent(Shader& shader)
	{
		auto pos = glm::vec3{
			chunkCoords.x * ChunkWidth,
			chunkCoords.y * ChunkHeight,
			chunkCoords.z * ChunkDepth,
		};
		shader.setMat4("model", glm::translate(glm::mat4{1.f}, pos));
		translucentMesh.draw();
	}

	static constexpr int ChunkWidth = 16;  // x
	static constexpr int ChunkHeight = 256;  // y
	static constexpr int ChunkDepth = 16;  // z

	static constexpr int WaterHeight = 80;

	void makeMesh()
	{
		makeOpaqueMesh();
		makeTranslucentMesh();
	}

	void makeOpaqueMesh();
	void makeTranslucentMesh();

private:
	World* world;

	Coords chunkCoords;

	BasicMesh opaqueMesh;
	BasicMesh translucentMesh;

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
		DU::floorDiv(globalCoords.x, Chunk::ChunkWidth),
		DU::floorDiv(globalCoords.y, Chunk::ChunkHeight),
		DU::floorDiv(globalCoords.z, Chunk::ChunkDepth)
	};
}

#endif // SRC_CHUNK_H_
