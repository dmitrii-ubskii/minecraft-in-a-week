#ifndef SRC_OPENGL_BASIC_MESH_H_
#define SRC_OPENGL_BASIC_MESH_H_

#include <vector>

struct Vertex
{
	struct Position
	{
		float x;
		float y;
		float z;
	} position;
	struct TextureCoords
	{
		float u;
		float v;
	} textureCoords;
};

class BasicMesh
{
public:
	BasicMesh(std::initializer_list<Vertex>, std::initializer_list<unsigned> indices_);

	void draw();

private:
	unsigned VAO;
	unsigned VBO;
	unsigned EBO;

	std::vector<Vertex> vertices;
	std::vector<unsigned> indices;
};

#endif // SRC_OPENGL_BASIC_MESH_H_