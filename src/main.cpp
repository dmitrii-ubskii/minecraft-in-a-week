#include <array>
#include <iostream>

#include <glm/gtx/transform.hpp>

#include <stb_image.h>

#include "opengl/camera.h"
#include "opengl/context.h"
#include "opengl/shader.h"

#include "cube.h"

int main()
{
	auto context = GlContext{800, 600, "Minecraft in a week"};

	auto quadShader = Shader{"shaders/quad.vert", "shaders/quad.frag"};
	Camera camera;

	auto cube = Cube{0, 1, 2};

	// Atlas stuff
	unsigned textureId;
	{
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
	
		// nearest neighbour texture scaling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// no texture wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		int width, height, nrChannels;
		unsigned char* data = stbi_load("data/atlas.png", &width, &height, &nrChannels, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
	}

	context.resetDeltaTime();
	auto time = 0.f;

	while (not context.shouldClose())
	{
		context.clear();
		auto deltaTime = context.getDeltaTime();
		time += deltaTime;
		auto velocity = deltaTime * 2.5f;
		
		if (context.getKey(GLFW_KEY_ESCAPE) == GLFW_PRESS)
			context.setShouldClose(true);

		if (context.getKey(GLFW_KEY_W) == GLFW_PRESS)
			camera.move(velocity, 0);
		if (context.getKey(GLFW_KEY_S) == GLFW_PRESS)
			camera.move(-velocity, 0);
		if (context.getKey(GLFW_KEY_A) == GLFW_PRESS)
			camera.move(0, -velocity);
		if (context.getKey(GLFW_KEY_D) == GLFW_PRESS)
			camera.move(0, velocity);

		if (context.getKey(GLFW_KEY_UP) == GLFW_PRESS)
			camera.rotate(velocity * 10, 0);
		if (context.getKey(GLFW_KEY_DOWN) == GLFW_PRESS)
			camera.rotate(-velocity * 10, 0);
		if (context.getKey(GLFW_KEY_LEFT) == GLFW_PRESS)
			camera.rotate(0, -velocity * 10);
		if (context.getKey(GLFW_KEY_RIGHT) == GLFW_PRESS)
			camera.rotate(0, velocity * 10);

		quadShader.use();
		quadShader.setMat4("model", glm::rotate(
				glm::scale(glm::mat4{1.f}, glm::vec3{1.5f}),
				glm::radians(time * 50.f), glm::vec3{0.f, 1.f, 0.f}
			)
		);
		quadShader.setMat4("view", camera.getViewMatrix());
		quadShader.setMat4("projection", camera.getProjectionMatrix());
		quadShader.setInt("textureAtlas", 0);
		cube.draw();

		context.swapBuffers();
		context.pollEvents();
	}

	return 0;
}

