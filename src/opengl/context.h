#ifndef SRC_OPENGL_CONTEXT_H_
#define SRC_OPENGL_CONTEXT_H_

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class GlContext
{
public:
	GlContext(int width, int height, std::string const& title);
	~GlContext();

	bool shouldClose() const { return glfwWindowShouldClose(window); }
	void setShouldClose(bool value) { glfwSetWindowShouldClose(window, value); }

	void clear();

	void resetDeltaTime() { lastFrame = (float)glfwGetTime(); }
	float getDeltaTime()
	{
		auto deltaTime = (float)glfwGetTime() - lastFrame;
		resetDeltaTime();
		return deltaTime;
	}

	void swapBuffers() { glfwSwapBuffers(window); }
	void pollEvents() { glfwPollEvents(); }
	int getKey(int key) { return glfwGetKey(window, key); }


private:
	GLFWwindow* window;

	float lastFrame;
};

#endif // SRC_OPENGL_CONTEXT_H_
