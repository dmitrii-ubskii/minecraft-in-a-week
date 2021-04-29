#include "opengl/basic_mesh.h"

#include <glad/glad.h>

BasicMesh::BasicMesh(std::initializer_list<Vertex> vertices_, std::initializer_list<unsigned> indices_)
	: vertices{vertices_}
	, indices{indices_}
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	auto bufferSize = static_cast<int>(vertices.size() * sizeof(Vertex));
	glBufferData(GL_ARRAY_BUFFER, bufferSize, vertices.data(), GL_STATIC_DRAW);

	auto positionSize = static_cast<int>(sizeof(Vertex::Position) / sizeof(float));
	glVertexAttribPointer(0, positionSize, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	glEnableVertexAttribArray(0);

	auto coordsSize = static_cast<int>(sizeof(Vertex::TextureCoords) / sizeof(float));
	glVertexAttribPointer(1, coordsSize, GL_FLOAT, GL_FALSE, sizeof(Vertex), (float*)(sizeof(Vertex::Position)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<int>(indices.size() * sizeof(unsigned)), indices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);
}

void BasicMesh::draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
