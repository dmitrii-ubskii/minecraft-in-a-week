#ifndef SRC_WORLD_H_
#define SRC_WORLD_H_

#include <map>

#include <glm/glm.hpp>

#include "chunk.h"
#include "seeded_noise.h"

class World
{
public:
	World(int seed): noiseDevice{seed} {}

	bool isGenerated(std::pair<int, int> coords)
	{
		return chunks.contains(coords);
	}

	void generateChunk(std::pair<int, int> coords)
	{
		auto [cx, cz] = coords;
		chunks.insert({std::pair{cx, cz}, Chunk{cx, cz, noiseDevice}});
	}

	void draw(Shader& shader, Coords playerChunk, glm::vec3 cameraDirection, int drawDistance)
	{
		for (auto& [coords, chunk]: chunks)
		{
			auto [x, z] = coords;
			auto dx = playerChunk.x - x;
			auto dz = playerChunk.z - z;
			auto squareDist = dx * dx + dz * dz;
			if (squareDist > drawDistance * drawDistance)
				continue;
			if (squareDist > 9)
			{
				auto direction = glm::normalize(glm::vec3{x - playerChunk.x, 0, z - playerChunk.z});
				if (glm::dot(direction, cameraDirection) < 0.25f)
					continue;
			}
			chunk.draw(shader);
		}
	}

	Chunk& at(std::pair<int, int> coords)
	{
		return chunks.at(coords);
	}

	void updateMesh(std::pair<int, int> coords)
	{
		chunks.at(coords).makeMesh();
	}

private:
	Noise noiseDevice{123456};
	std::map<std::pair<int, int>, Chunk> chunks;
};

#endif // SRC_WORLD_H_
