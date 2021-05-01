#include <array>
#include <iostream>

#include <glm/gtx/transform.hpp>
#include <glm/gtc/noise.hpp>

#include <stb_image.h>

#include "opengl/camera.h"
#include "opengl/context.h"
#include "opengl/shader.h"

#include "cube.h"

int main()
{
	auto context = GlContext{800, 600, "Minecraft in a week"};

	auto blockShader = Shader{"shaders/block.vert", "shaders/block.frag"};
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

	auto const side = 64;
	auto const scale = 8;
	std::vector<glm::vec3> positions;
	positions.reserve(side * side * scale);
	for (auto i = 0; i < side; i++)
	{
		for (auto j = 0; j < side; j++)
		{
			auto x = (float)i;
			auto z = (float)j;
			auto altitude = (int)std::floor((1 + glm::simplex(glm::vec3{12356.f, (float)i / 32.f, (float)j / 32.f})) * (float)scale);
			for (auto y = 0; y < altitude; y++)
			{
				positions.emplace_back(x, y - 2*scale, z);
			}
		}
	}

	context.resetDeltaTime();
	auto time = 0.f;

	context.grabMouse();
	auto const mouseSensitivity = 0.3f;

	while (not context.shouldClose())
	{
		context.clear();
		auto deltaTime = context.getDeltaTime();
		time += deltaTime;
		auto velocity = deltaTime * 5.f;
		
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

		camera.rotate(
			-(float)context.getCursorDeltaY() * mouseSensitivity,
			(float)context.getCursorDeltaX() * mouseSensitivity
		);

		blockShader.use();
		blockShader.setMat4("view", camera.getViewMatrix());
		blockShader.setMat4("projection", camera.getProjectionMatrix());
		blockShader.setInt("textureAtlas", 0);
		for (auto const& pos: positions)
		{
			blockShader.setMat4("model", glm::translate(glm::scale(glm::mat4{1.f}, glm::vec3{0.5f}), pos));
			cube.draw();
		}

		context.swapBuffers();
		context.pollEvents();
	}

	return 0;
}

