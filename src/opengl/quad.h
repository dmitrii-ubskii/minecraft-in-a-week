#ifndef SRC_OPENGL_QUAD_H_
#define SRC_OPENGL_QUAD_H_

#include <glad/glad.h>

#include "opengl/shader.h"

inline void drawQuad(GLuint textureId, int centerX, int centerY, int width, int height)
{
	static auto quadShader = Shader{"shaders/quad.vert", "shaders/quad.frag"};
	static float quadVertices[] = {
        // positions          // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    static unsigned int quadIndices[] = {
        0, 3, 1,
        1, 3, 2
    };
    static unsigned int quadVBO, quadVAO, quadEBO;
	static bool initialized = false;

	if (not initialized)
    {
		glGenVertexArrays(1, &quadVAO);

		glBindVertexArray(quadVAO);
		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &quadEBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

		initialized = true;
    }

	// FIXME assuming 800x600
	auto normalizedX = (float)centerX / 400.f - 1.f;
	auto normalizedY = (float)centerY / 300.f - 1.f;

	auto normalizedWidth = (float)width / 800.f;
	auto normalizedHeight = (float)height / 600.f;

	quadShader.use();
	quadShader.setVec2("position", {normalizedX, normalizedY});
	quadShader.setVec2("scale", {normalizedWidth, normalizedHeight});

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	quadShader.setInt("image", 0);

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, std::size(quadIndices), GL_UNSIGNED_INT, 0);
}

#endif // SRC_OPENGL_QUAD_H_
