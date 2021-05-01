#include <array>
#include <iostream>
#include <map>

#include <stb_image.h>

#include "opengl/camera.h"
#include "opengl/context.h"
#include "opengl/shader.h"

#include "chunk.h"
#include "seeded_noise.h"

int main()
{
	auto context = GlContext{800, 600, "Minecraft in a week"};

	auto blockShader = Shader{"shaders/block.vert", "shaders/block.frag"};
	Camera camera;

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

	Noise noiseDevice{123456};
	auto chunks = std::map<std::pair<int, int>, Chunk>{};

	context.resetDeltaTime();
	auto time = 0.f;

	context.grabMouse();
	auto const mouseSensitivity = 0.3f;
	auto const drawDistance = 10;

	while (not context.shouldClose())
	{
		context.clear();
		auto deltaTime = context.getDeltaTime();
		time += deltaTime;
		auto velocity = deltaTime * 15.f;
		std::cerr << (int)(1.f/deltaTime) << " FPS\n";
		
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

		auto cameraPos = camera.getPosition();
		auto playerChunk = getBlockChunk({(int)cameraPos.x, (int)cameraPos.y, (int)cameraPos.z});
		for (auto delta = 0; delta <= drawDistance; delta++)
		{
			bool generated = false;
			for (auto dx = -delta; dx <= delta && not generated; dx++)
			{
				for (auto dz = -delta; dz <= delta && not generated; dz++)
				{
					auto x = playerChunk.x + dx;
					auto z = playerChunk.z + dz;
					if (not chunks.contains({x, z}))
					{
						chunks.insert({std::pair{x, z}, Chunk{x, z, noiseDevice}});
						generated = true;
					}
				}
			}
			if (generated)
				break;
		}

		for (auto& [coords, chunk]: chunks)
		{
			if (std::abs(playerChunk.x - coords.first) > drawDistance ||
					std::abs(playerChunk.z - coords.second) > drawDistance)
				continue;
			chunk.draw(blockShader);
		}

		context.swapBuffers();
		context.pollEvents();
	}

	return 0;
}

