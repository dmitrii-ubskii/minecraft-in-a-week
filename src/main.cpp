#include <array>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "opengl/camera.h"
#include "opengl/shader.h"
#include "opengl/window.h"

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto window = Window{800, 600, "Minecraft in a week"};
	window.makeContextCurrent();

	if (not gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD\n";
		return -1;
	}

	glViewport(0, 0, 800, 600);

	auto const quadVertices = std::array{
		-0.8f, -0.8f, 1.f, 0.f, 0.f,
		0.8f, -0.8f, 0.f, 1.f, 1.f,
		-0.8f, 0.8f, 1.f, 0.f, 1.f,

		-0.8f, 0.8f, 1.f, 0.f, 1.f,
		0.8f, -0.8f, 0.f, 1.f, 1.f,
		0.8f, 0.8f, 0.f, 1.f, 0.f,
	};

	unsigned VAO;
	glGenVertexArrays(1, &VAO);
	unsigned VBO;
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof quadVertices, quadVertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), nullptr);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (float*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	auto quadShader = Shader{"shaders/quad.vert", "shaders/quad.frag"};

	Camera camera;

	auto lastFrame = (float)glfwGetTime();
	while (not window.shouldClose())
	{
		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto time = (float)glfwGetTime();
		auto deltaTime = time - lastFrame;
		auto velocity = deltaTime * 2.5f;
		lastFrame = time;

		if (window.getKey(GLFW_KEY_ESCAPE) == GLFW_PRESS)
			window.setShouldClose(true);

		if (window.getKey(GLFW_KEY_W) == GLFW_PRESS)
			camera.move(velocity, 0);
		if (window.getKey(GLFW_KEY_S) == GLFW_PRESS)
			camera.move(-velocity, 0);
		if (window.getKey(GLFW_KEY_A) == GLFW_PRESS)
			camera.move(0, -velocity);
		if (window.getKey(GLFW_KEY_D) == GLFW_PRESS)
			camera.move(0, velocity);

		if (window.getKey(GLFW_KEY_UP) == GLFW_PRESS)
			camera.rotate(velocity * 10, 0);
		if (window.getKey(GLFW_KEY_DOWN) == GLFW_PRESS)
			camera.rotate(-velocity * 10, 0);
		if (window.getKey(GLFW_KEY_LEFT) == GLFW_PRESS)
			camera.rotate(0, -velocity * 10);
		if (window.getKey(GLFW_KEY_RIGHT) == GLFW_PRESS)
			camera.rotate(0, velocity * 10);

		quadShader.use();
		quadShader.setMat4("model", glm::rotate(
				glm::scale(glm::mat4{1.f}, glm::vec3{0.5f}),
				glm::radians(time * 50.f), glm::vec3{0.f, 0.f, 1.f}
			)
		);
		quadShader.setMat4("view", camera.getViewMatrix());
		quadShader.setMat4("projection", camera.getProjectionMatrix());

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		window.swapBuffers();
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}
