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
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	Noise noiseDevice{123456};
	auto chunks = std::map<std::pair<int, int>, Chunk>{};

	context.resetDeltaTime();
	auto time = 0.f;

	context.grabMouse();
	auto const mouseSensitivity = 0.3f;
	auto const drawDistance = 10;
	auto const lightDirection = glm::normalize(glm::vec3{2, -3, 1});

	auto shadowShader = Shader{"shaders/shadow_map.vert", "shaders/shadow_map.frag"};

	unsigned shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);
	unsigned shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 4096, 4096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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

		auto lightPosition = glm::vec3{cameraPos.x - lightDirection.x * 100.f, 256.f, cameraPos.z - lightDirection.z * 100.f};
		auto lightSpace = glm::ortho(-100.f, 100.f, -100.f, 100.f, 1.f, 1000.f) *
			glm::lookAt(lightPosition, lightPosition + lightDirection, glm::vec3{0.f, 1.f, 0.f});

		// render shadow map
		glViewport(0, 0, 4096, 4096);
		glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		shadowShader.use();
		shadowShader.setMat4("lightSpace", lightSpace);
		shadowShader.setVec3("lightDirection", lightDirection);
		for (auto& [coords, chunk]: chunks)
		{
			if (std::abs(playerChunk.x - coords.first) > drawDistance ||
					std::abs(playerChunk.z - coords.second) > drawDistance)
				continue;
			chunk.draw(shadowShader);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		context.setViewport();
		blockShader.use();
		blockShader.setMat4("view", camera.getViewMatrix());
		blockShader.setMat4("projection", camera.getProjectionMatrix());
		blockShader.setMat4("lightSpace", lightSpace);
		blockShader.setVec3("lightDirection", lightDirection);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		blockShader.setInt("textureAtlas", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		blockShader.setInt("shadowMap", 1);
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

