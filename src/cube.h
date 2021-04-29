#ifndef SRC_CUBE_H_
#define SRC_CUBE_H_

#include "opengl/basic_mesh.h"

class Cube: public BasicMesh
{
public:
	Cube(int topTexture, int sideTexture, int bottomTexture);
};

#endif // SRC_CUBE_H_
