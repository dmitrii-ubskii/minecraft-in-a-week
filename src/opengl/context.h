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

	void setViewport() { glViewport(0, 0, 800, 600); }

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

	void grabMouse()
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwGetCursorPos(window, &cursorX, &cursorY);
		cursorDeltaX = 0;
		cursorDeltaY = 0;
	}

	void pollEvents();
	int getKey(int key) { return glfwGetKey(window, key); }
	int getMouseButton(int button) { return glfwGetMouseButton(window, button); }
	GLdouble getCursorDeltaX() { return cursorDeltaX; }
	GLdouble getCursorDeltaY() { return cursorDeltaY; }

private:
	GLFWwindow* window;

	float lastFrame;

	GLdouble cursorX;
	GLdouble cursorY;

	GLdouble cursorDeltaX;
	GLdouble cursorDeltaY;
};

#endif // SRC_OPENGL_CONTEXT_H_
