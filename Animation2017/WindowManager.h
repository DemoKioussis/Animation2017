#pragma once

#include <glad\glad.h>
#include <glm\glm.hpp>
#include "TimeSystem.h"
#include "RenderEngine.h"
#include "InputManager.h"


class InputManager;

/*
	This class will handle all window operations
*/
class WindowManager {


private:
	GLuint WIDTH, HEIGHT;
	GLFWwindow* window;
	glm::vec4 backgroundColor;

	int framerate = 60;
	float refreshRate = 1.0f / 60.0f;

	int framesThisSecond = 0;
	float lastSecond;

	GLFWwindow* makeWindow(int width, int height) {

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


		glfwWindowHint(GLFW_SAMPLES, 0);
		glfwWindowHint(GLFW_RED_BITS, 8);
		glfwWindowHint(GLFW_GREEN_BITS, 8);
		glfwWindowHint(GLFW_BLUE_BITS, 8);
		glfwWindowHint(GLFW_ALPHA_BITS, 8);
		glfwWindowHint(GLFW_STENCIL_BITS, 8);
		glfwWindowHint(GLFW_DEPTH_BITS, 24);
		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

		// glfw window creation
		// --------------------
		GLFWwindow* window = glfwCreateWindow(width, height, "Physics Simulation", NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
		}
		glfwMakeContextCurrent(window);

		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
		}
		glEnable(GL_DEPTH_TEST);



		return window;

	}

public:


	WindowManager(GLuint w, GLuint h) {
		window = makeWindow(w, h);
		WIDTH = w;
		HEIGHT = h;
		backgroundColor = glm::vec4(0, 0, 0, 1);

		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		//glfwSetCursorPos(window, WIDTH / 2, HEIGHT / 2);
	}
	~WindowManager() {

	}

	void setDimensions(GLuint w, GLuint h) {
		WIDTH = w;
		HEIGHT = h;
	}
	void setBackgroundColor(glm::vec4 c) {
		backgroundColor = c;
	}

	bool windowHasClosed() {
		return !glfwWindowShouldClose(window);
	}

	GLFWwindow* getWindow() {
		return window;
	}
	const GLuint width() {
		return WIDTH;
	}
	const GLuint height() {
		return HEIGHT;
	}
	void setFramerate(int i) {
		framerate = i;
		refreshRate = (float)1 / (float)i;
	}
	void clearWindow() {
		glClearColor(backgroundColor.x,backgroundColor.y,backgroundColor.z,backgroundColor.a);
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		
	}
	void swapBuffer() {
		glfwSwapBuffers(window);
	}

	


	void frameTick() {
		if (TimeSystem::frameCheck() >= refreshRate) {
			InputManager::processInput(); // Not sure if this should be here. Might aswell be inside the physical step tick.
			refreshWindow();
			TimeSystem::frameStep();
		}
	}

	void refreshWindow() {
		clearWindow();
		drawMeshes();
		//swapBuffer();

	}
	// we will extend this funciton to take in multiple inputs

	void drawMeshes() {
		((RenderEngine*)RenderEngine::getInstance())->drawRenderComponents();
	}



};