#include "opengl/context.h"

#include <iostream>

GlContext::GlContext(int width, int height, std::string const& title)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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

	setViewport();
	glDisable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	resetDeltaTime();
}

GlContext::~GlContext()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void GlContext::clear()
{
	glClearColor(0.5f, 0.8f, 0.9f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GlContext::pollEvents()
{
	glfwPollEvents();

	GLdouble newCursorX;
	GLdouble newCursorY;
	glfwGetCursorPos(window, &newCursorX, &newCursorY);

	cursorDeltaX = newCursorX - cursorX;
	cursorDeltaY = newCursorY - cursorY;

	cursorX = newCursorX;
	cursorY = newCursorY;
}

