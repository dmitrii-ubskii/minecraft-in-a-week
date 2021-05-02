#include <array>
#include <iostream>
#include <map>
#include <optional>

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
	unsigned textureAtlas;
	{
		glGenTextures(1, &textureAtlas);
		glBindTexture(GL_TEXTURE_2D, textureAtlas);
	
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

	// selected block highlight
	auto selectionShader = Shader{"shaders/selection.vert", "shaders/selection.frag"};
	float vertices[] = {
		0.f, 0.f, 0.f,
		0.f, 0.f, 1.f,
		0.f, 1.f, 0.f,
		0.f, 1.f, 1.f,
		1.f, 0.f, 0.f,
		1.f, 0.f, 1.f,
		1.f, 1.f, 0.f,
		1.f, 1.f, 1.f,
    };
    unsigned int indices[] = {
		0, 1, 3, 2,
		0, 4, 5, 7,
		6, 2, 3, 7,
		6, 4, 5, 1
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

	// quad
	auto quadShader = Shader{"shaders/quad.vert", "shaders/quad.frag"};
	float quadVertices[] = {
        // positions          // texture coords
         1.0f,  1.0f, 0.0f,   1.0f, 1.0f, // top right
         1.0f, -1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, // bottom left
        -1.0f,  1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int quadIndices[] = {
        0, 3, 1,
        1, 3, 2
    };
    unsigned int quadVBO, quadVAO, quadEBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glGenBuffers(1, &quadEBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
	// end quad

	// Atlas stuff
	unsigned crosshair;
	{
		glGenTextures(1, &crosshair);
		glBindTexture(GL_TEXTURE_2D, crosshair);
	
		// nearest neighbour texture scaling
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// no texture wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		int width, height, nrChannels;
		unsigned char* data = stbi_load("data/crosshair.png", &width, &height, &nrChannels, 4);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(data);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	while (not context.shouldClose())
	{
		context.clear();
		auto deltaTime = context.getDeltaTime();
		time += deltaTime;
		auto velocity = deltaTime * 35.f;
		std::cerr << "             \r" << (int)(1.f/deltaTime) << " FPS\r";
		
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
			auto dx = playerChunk.x - x;
			auto dz = playerChunk.z - z;
			auto squareDist = dx * dx + dz * dz;
			if (squareDist > drawDistance * drawDistance)
				continue;
			if (squareDist > 9)
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
		blockShader.setFloat("renderDistance", drawDistance * 16.f);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureAtlas);
		blockShader.setInt("textureAtlas", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, shadowMap);
		blockShader.setInt("shadowMap", 1);
		for (auto& [coords, chunk]: chunks)
		{
			auto [x, z] = coords;
			auto dx = playerChunk.x - x;
			auto dz = playerChunk.z - z;
			auto squareDist = dx * dx + dz * dz;
			if (squareDist > drawDistance * drawDistance)
				continue;
			if (squareDist > 9)
			{
				auto direction = glm::normalize(glm::vec3{x - playerChunk.x, 0, z - playerChunk.z});
				if (glm::dot(direction, camera.getDirection()) < 0)
					continue;
			}
			chunk.draw(blockShader);
		}

		// ray casting
		auto placementBlock = std::optional<Coords>{};
		auto activeBlock = std::optional<Coords>{};
		for (auto t = 0; t < 32; t++)
		{
			auto fBlock = cameraPos + (float)t * 0.5f * camera.getDirection();
			auto block = Coords{
				std::floor(fBlock.x),
				std::floor(fBlock.y),
				std::floor(fBlock.z)
			};
			if (block.y > 255) break;
			auto chunkCoords = getBlockChunk(block);
			if (chunks.contains({chunkCoords.x, chunkCoords.z}))
			{
				if (chunks.at({chunkCoords.x, chunkCoords.z})[block] != BlockType::Air)
				{
					activeBlock = block;
					break;
				}
			}
			else
			{
				break;
			}
			placementBlock = block;
		}

		if (activeBlock.has_value())
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			selectionShader.use();
			selectionShader.setMat4("view", camera.getViewMatrix());
			selectionShader.setMat4("projection", camera.getProjectionMatrix());
			selectionShader.setMat4("model", glm::translate(glm::mat4{1.f}, glm::vec3{*activeBlock}));
			glBindVertexArray(VAO);
			glDrawElements(GL_LINE_STRIP, std::size(indices), GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			if (context.getMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
			{
				auto chunkCoords = getBlockChunk(*activeBlock);
				chunks.at({chunkCoords.x, chunkCoords.z})[*activeBlock] = BlockType::Air;
				chunks.at({chunkCoords.x, chunkCoords.z}).makeMesh();
			}
			if (context.getMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
			{
				auto chunkCoords = getBlockChunk(*placementBlock);
				chunks.at({chunkCoords.x, chunkCoords.z})[*placementBlock] = BlockType::Dirt;
				chunks.at({chunkCoords.x, chunkCoords.z}).makeMesh();
			}
		}

		quadShader.use();
		quadShader.setVec2("scale", {16.f/800.f, 16.f/600.f});
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, crosshair);
		quadShader.setInt("image", 0);
		glBindVertexArray(quadVAO);
		glDrawElements(GL_TRIANGLES, std::size(quadIndices), GL_UNSIGNED_INT, 0);

		context.swapBuffers();
		context.pollEvents();
	}

	return 0;
}

