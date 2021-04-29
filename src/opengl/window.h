#ifndef SRC_OPENGL_WINDOW_H_
#define SRC_OPENGL_WINDOW_H_

#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window(int width, int height, std::string caption);
	void makeContextCurrent();

	bool shouldClose() const;
	void setShouldClose(bool value);

	int getKey(int key);

	void swapBuffers();

private:
	GLFWwindow* window;
};

#endif // SRC_OPENGL_WINDOW_H_
