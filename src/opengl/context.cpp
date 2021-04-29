#include "opengl/context.h"

#include <iostream>

GlContext::GlContext(int width, int height, std::string const& title)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

	if (window == nullptr)
	{
		std::cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		throw;
	}

	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);

	if (not gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD\n";
		glfwTerminate();
		throw;
	}

	glViewport(0, 0, 800, 600);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);

	resetDeltaTime();
}

GlContext::~GlContext()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void GlContext::clear()
{
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
