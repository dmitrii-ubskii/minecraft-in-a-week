#ifndef SRC_WORLD_H_
#define SRC_WORLD_H_

#include <map>
#include <set>

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
		chunks.insert({std::pair{cx, cz}, Chunk{this, cx, cz, noiseDevice}});
		for (auto dx = -1; dx <= 1; dx++)
			for (auto dz = -1; dz <= 1; dz++)
			{
				auto x = cx + dx;
				auto z = cz + dz;
				if (isGenerated({x, z}))
				{
					remeshBacklog.insert({x, z});
				}
			}
	}

	void drawOpaque(Shader& shader, Coords playerChunk, glm::vec3 cameraDirection, int drawDistance)
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
			chunk.drawOpaque(shader);
		}
	}

	void draw(Shader& shader, Coords playerChunk, glm::vec3 cameraDirection, int drawDistance)
	{
		drawOpaque(shader, playerChunk, cameraDirection, drawDistance);
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
			chunk.drawTranslucent(shader);
		}
	}

	Chunk& at(std::pair<int, int> coords)
	{
		return chunks.at(coords);
	}

	void updateMesh(std::pair<int, int> coords)
	{
		remeshBacklog.insert(coords);
	}

	void tick(Coords playerChunk, int drawDistance)
	{
		if (remeshBacklog.empty())
		{
			return;
		}
		for (auto i = 0, x = 0, z = 0, dx = 0, dz = -1;
			 i < (2 * drawDistance + 1) * (2 * drawDistance + 1);
			 i++, x += dx, z += dz)
		{
			auto cx = playerChunk.x + x;
			auto cz = playerChunk.z + z;
			if (remeshBacklog.contains({cx, cz}))
			{
				auto it = remeshBacklog.find({cx, cz});
				at(*it).makeMesh();
				remeshBacklog.erase(it);
				return;
			}

			if (x == z || (x == -z && x < 0) || (x == 1-z && x > 0))  // if at a diagonal
			{
				// rotate direction
				auto t = dx;
				dx = -dz;
				dz = t;
			}
		}

		// remesh a random chunk in the backlog
		at(*remeshBacklog.begin()).makeMesh();
		remeshBacklog.erase(remeshBacklog.begin());
	}

private:
	Noise noiseDevice{123456};
	std::map<std::pair<int, int>, Chunk> chunks;

	std::set<std::pair<int, int>> remeshBacklog;
};

#endif // SRC_WORLD_H_
