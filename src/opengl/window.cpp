#include "opengl/window.h"

#include <iostream>

Window::Window(int width, int height, std::string caption)
	: window{glfwCreateWindow(width, height, caption.c_str(), nullptr, nullptr)}
{
	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		throw;
	}

	glfwSetWindowUserPointer(window, this);
}

void Window::setShouldClose(bool value)
{
	glfwSetWindowShouldClose(window, value);
}

int Window::getKey(int key)
{
	return glfwGetKey(window, key);
}

void Window::makeContextCurrent()
{
	glfwMakeContextCurrent(window);
}

bool Window::shouldClose() const
{
	return glfwWindowShouldClose(window);
}

void Window::swapBuffers()
{
	glfwSwapBuffers(window);
}
