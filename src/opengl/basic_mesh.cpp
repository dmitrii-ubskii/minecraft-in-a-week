#include "opengl/basic_mesh.h"

#include <glad/glad.h>

BasicMesh::BasicMesh(std::initializer_list<Vertex> vertices, std::initializer_list<unsigned> indices)
	: BasicMesh{vertices.begin(), vertices.size(), indices.begin(), indices.size()}
{}

BasicMesh::BasicMesh(std::vector<Vertex> const& vertices, std::vector<unsigned> const& indices)
	: BasicMesh{&*vertices.begin(), vertices.size(), &*indices.begin(), indices.size()}
{}

BasicMesh::BasicMesh(Vertex const* vertices, std::size_t numVertices, unsigned const* indices, std::size_t numIndices_)
	: numIndices{(int)numIndices_}
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	auto bufferSize = static_cast<int>(numVertices * sizeof(Vertex));
	glBufferData(GL_ARRAY_BUFFER, bufferSize, vertices, GL_STATIC_DRAW);

	auto positionSize = static_cast<int>(sizeof(Vertex::Position) / sizeof(float));
	glVertexAttribPointer(0, positionSize, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glEnableVertexAttribArray(0);

	auto coordsSize = static_cast<int>(sizeof(Vertex::TextureCoords) / sizeof(float));
	glVertexAttribPointer(1, coordsSize, GL_FLOAT, GL_FALSE, sizeof(Vertex), (float*)(sizeof(Vertex::Position)));
	glEnableVertexAttribArray(1);

	auto normalSize = static_cast<int>(sizeof(Vertex::Normal) / sizeof(float));
	glVertexAttribPointer(2, normalSize, GL_FLOAT, GL_FALSE, sizeof(Vertex), (float*)(sizeof(Vertex::Position) + sizeof(Vertex::TextureCoords)));
	glEnableVertexAttribArray(2);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<int>(numIndices_ * sizeof(unsigned)), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void BasicMesh::draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
