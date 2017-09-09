#pragma once

#include <GLFW/glfw3.h>
#include <glm\glm.hpp>



/*
	This class will handle all window operations
*/
class WindowManager {

private:

	GLFWwindow* window;
	glm::vec4 backgroundColor;


public:

	WindowManager() {
		backgroundColor = glm::vec4(0, 0, 0, 1);
	}
	~WindowManager() {

	}


	void setWindow(GLFWwindow* w) {
		window = w;
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

	void clearWindow() {
		processInput();
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.a);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	void swapBuffer() {
		glfwSwapBuffers(window);
	}
	void pollEvents() {
		glfwPollEvents();
	}


	// we will extend this funciton to take in multiple inputs
	void processInput() {

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

	}


};