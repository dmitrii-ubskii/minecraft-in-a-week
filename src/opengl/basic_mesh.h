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
	struct Normal
	{
		float x;
		float y;
		float z;
	} normal;
};

class BasicMesh
{
public:
	BasicMesh() {}
	BasicMesh(std::initializer_list<Vertex>, std::initializer_list<unsigned> indices);
	BasicMesh(std::vector<Vertex> const&, std::vector<unsigned> const& indices);

	void draw();

private:
	BasicMesh(Vertex const*, std::size_t, unsigned const*, std::size_t);

	unsigned VAO;
	unsigned VBO;
	unsigned EBO;

	int numIndices;
};

#endif // SRC_OPENGL_BASIC_MESH_H_
