#include "chunk.h"

#include "world.h"

void Chunk::makeOpaqueMesh()
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
				if (not isOpaque(blocks[index]))
					continue;

				bool doLeft = [&](){
					if (x == 0)
					{
						auto leftChunk = std::pair{chunkCoords.x - 1, chunkCoords.z};
						if (world->isGenerated(leftChunk))
						{
							return not isOpaque(world->at(leftChunk).blocks.at(localCoordsToIndex({ChunkWidth - 1, y, z})));
						}
						else
						{
							return true;
						}
					}
					else
					{
						return not isOpaque(blocks[localCoordsToIndex({x - 1, y, z})]);
					}
				}();
				if (doLeft)
				{
					auto faceVertices = cubes.at(blocks[index]).getLeftFace({x, y, z});
					std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
				}

				bool doRight = [&](){
					if (x == ChunkWidth - 1)
					{
						auto rightChunk = std::pair{chunkCoords.x + 1, chunkCoords.z};
						if (world->isGenerated(rightChunk))
						{
							return not isOpaque(world->at(rightChunk).blocks.at(localCoordsToIndex({0, y, z})));
						}
						else
						{
							return true;
						}
					}
					else
					{
						return not isOpaque(blocks[localCoordsToIndex({x + 1, y, z})]);
					}
				}();
				if (doRight)
				{
					auto faceVertices = cubes.at(blocks[index]).getRightFace({x, y, z});
					std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
				}

				if (y == 0 || not isOpaque(blocks[localCoordsToIndex({x, y - 1, z})]))
				{
					auto faceVertices = cubes.at(blocks[index]).getBottomFace({x, y, z});
					std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
				}
				if (y == ChunkHeight - 1 || not isOpaque(blocks[localCoordsToIndex({x, y + 1, z})]))
				{
					auto faceVertices = cubes.at(blocks[index]).getTopFace({x, y, z});
					std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
				}

				bool doFront = [&](){
					if (z == 0)
					{
						auto frontChunk = std::pair{chunkCoords.x, chunkCoords.z - 1};
						if (world->isGenerated(frontChunk))
						{
							return not isOpaque(world->at(frontChunk).blocks.at(localCoordsToIndex({x, y, ChunkDepth - 1})));
						}
						else
						{
							return true;
						}
					}
					else
					{
						return not isOpaque(blocks[localCoordsToIndex({x, y, z - 1})]);
					}
				}();
				if (doFront)
				{
					auto faceVertices = cubes.at(blocks[index]).getFrontFace({x, y, z});
					std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
				}
				bool doBack = [&](){
					if (z == ChunkDepth - 1)
					{
						auto backChunk = std::pair{chunkCoords.x, chunkCoords.z + 1};
						if (world->isGenerated(backChunk))
						{
							return not isOpaque(world->at(backChunk).blocks.at(localCoordsToIndex({x, y, 0})));
						}
						else
						{
							return true;
						}
					}
					else
					{
						return not isOpaque(blocks[localCoordsToIndex({x, y, z + 1})]);
					}
				}();
				if (doBack)
				{
					auto faceVertices = cubes.at(blocks[index]).getBackFace({x, y, z});
					std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
				}
			}
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
	opaqueMesh = BasicMesh{vertices, indices};
}

void Chunk::makeTranslucentMesh()
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
				if (not isTranslucent(blocks[index]))
					continue;

				bool doLeft = [&](){
					if (x == 0)
					{
						auto leftChunk = std::pair{chunkCoords.x - 1, chunkCoords.z};
						if (world->isGenerated(leftChunk))
						{
							return world->at(leftChunk).blocks.at(localCoordsToIndex({ChunkWidth - 1, y, z})) == BlockType::Air;
						}
						else
						{
							return true;
						}
					}
					else
					{
						return blocks[localCoordsToIndex({x - 1, y, z})] == BlockType::Air;
					}
				}();
				if (doLeft)
				{
					auto faceVertices = cubes.at(blocks[index]).getLeftFace({x, y, z});
					std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
				}

				bool doRight = [&](){
					if (x == ChunkWidth - 1)
					{
						auto rightChunk = std::pair{chunkCoords.x + 1, chunkCoords.z};
						if (world->isGenerated(rightChunk))
						{
							return world->at(rightChunk).blocks.at(localCoordsToIndex({0, y, z})) == BlockType::Air;
						}
						else
						{
							return true;
						}
					}
					else
					{
						return blocks[localCoordsToIndex({x + 1, y, z})] == BlockType::Air;
					}
				}();
				if (doRight)
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

				bool doFront = [&](){
					if (z == 0)
					{
						auto frontChunk = std::pair{chunkCoords.x, chunkCoords.z - 1};
						if (world->isGenerated(frontChunk))
						{
							return world->at(frontChunk).blocks.at(localCoordsToIndex({x, y, ChunkDepth - 1})) == BlockType::Air;
						}
						else
						{
							return true;
						}
					}
					else
					{
						return blocks[localCoordsToIndex({x, y, z - 1})] == BlockType::Air;
					}
				}();
				if (doFront)
				{
					auto faceVertices = cubes.at(blocks[index]).getFrontFace({x, y, z});
					std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
				}
				bool doBack = [&](){
					if (z == ChunkDepth - 1)
					{
						auto backChunk = std::pair{chunkCoords.x, chunkCoords.z + 1};
						if (world->isGenerated(backChunk))
						{
							return world->at(backChunk).blocks.at(localCoordsToIndex({x, y, 0})) == BlockType::Air;
						}
						else
						{
							return true;
						}
					}
					else
					{
						return blocks[localCoordsToIndex({x, y, z + 1})] == BlockType::Air;
					}
				}();
				if (doBack)
				{
					auto faceVertices = cubes.at(blocks[index]).getBackFace({x, y, z});
					std::copy(faceVertices.cbegin(), faceVertices.cend(), std::back_inserter(vertices));
				}
			}
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
	translucentMesh = BasicMesh{vertices, indices};
}

