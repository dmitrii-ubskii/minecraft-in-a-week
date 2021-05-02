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
	auto const lightDirection = glm::normalize(glm::vec3{6, -3, 1});

	auto shadowShader = Shader{"shaders/shadow_map.vert", "shaders/shadow_map.frag"};

	unsigned shadowMapFBO;
	glGenFramebuffers(1, &shadowMapFBO);
	unsigned shadowMap;
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 4096, 4096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, std::array{1.f, 1.f, 1.f, 1.f}.data());
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
		auto velocity = deltaTime * 35.f;
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
		bool generated = false;
		for (auto i = 0, x = 0, z = 0, dx = 0, dz = -1;
			 i < (2 * drawDistance + 1) * (2 * drawDistance + 1) && not generated;
			 i++, x += dx, z += dz)
		{
			auto cx = playerChunk.x + x;
			auto cz = playerChunk.z + z;
			if (not chunks.contains({cx, cz}))
			{
				chunks.insert({std::pair{cx, cz}, Chunk{cx, cz, noiseDevice}});
				generated = true;
			}

			if (x == z || (x == -z && x < 0) || (x == 1-z && x > 0))  // if at a diagonal
			{
				// rotate direction
				auto t = dx;
				dx = -dz;
				dz = t;
			}
		}

		auto lightPosition = glm::vec3{cameraPos.x - lightDirection.x * 100.f, 256.f, cameraPos.z - lightDirection.z * 100.f};
		auto lightSpace = glm::ortho(-200.f, 200.f, -200.f, 200.f, 1.f, 1000.f) *
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
			auto [x, z] = coords;
			if (std::abs(playerChunk.x - x) > drawDistance ||
					std::abs(playerChunk.z - z) > drawDistance)
				continue;
			if (std::abs(playerChunk.x - x) > 3 ||
					std::abs(playerChunk.z - z) > 3)
			{
				auto direction = glm::normalize(glm::vec3{x - playerChunk.x, 0, z - playerChunk.z});
				if (glm::dot(direction, camera.getDirection()) < 0.25f)
					continue;
			}
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
			auto [x, z] = coords;
			if (std::abs(playerChunk.x - x) > drawDistance ||
					std::abs(playerChunk.z - z) > drawDistance)
				continue;
			if (std::abs(playerChunk.x - x) > 3 ||
					std::abs(playerChunk.z - z) > 3)
			{
				auto direction = glm::normalize(glm::vec3{x - playerChunk.x, 0, z - playerChunk.z});
				if (glm::dot(direction, camera.getDirection()) < 0.25f)
					continue;
			}
			chunk.draw(blockShader);
		}

		context.swapBuffers();
		context.pollEvents();
	}

	return 0;
}

