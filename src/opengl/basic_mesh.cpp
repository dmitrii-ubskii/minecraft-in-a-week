#include "opengl/basic_mesh.h"

#include <glad/glad.h>

BasicMesh::BasicMesh(std::initializer_list<Vertex> vertices, std::initializer_list<unsigned> indices)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	auto bufferSize = static_cast<int>(vertices.size() * sizeof(Vertex));
	glBufferData(GL_ARRAY_BUFFER, bufferSize, vertices.begin(), GL_STATIC_DRAW);

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
	numIndices = static_cast<int>(indices.size());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<int>(indices.size() * sizeof(unsigned)), indices.begin(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void BasicMesh::draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
