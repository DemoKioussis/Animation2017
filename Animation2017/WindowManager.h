#pragma once

#include <GLFW/glfw3.h>
#include <glm\glm.hpp>
#include "Mesh.h"
#include "TimeSystem.h"



/*
	This class will handle all window operations
*/
class WindowManager {

private:

	GLFWwindow* window;
	glm::vec4 backgroundColor;

	int framerate = 60;
	float refreshRate = 1.0f / 60.0f;

	int framesThisSecond = 0;
	float lastSecond;

public:


	WindowManager() {
		backgroundColor = glm::vec4(0, 0, 0, 1);
	}
	~WindowManager() {

	}
	Mesh* mesh; // temporary


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

	void setFramerate(int i) {
		framerate = i;
		refreshRate = (float)1 / (float)i;
	}
	void clearWindow() {
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.a);
		glClear(GL_COLOR_BUFFER_BIT);
	}
	void swapBuffer() {
		glfwSwapBuffers(window);
	}

	
	void pollEvents() {
		glfwPollEvents();
	}

	void frameTick() {
		if (TimeSystem::frameCheck() >= refreshRate) {
			refreshWindow();
			TimeSystem::frameStep();

		}
	}

	void refreshWindow() {
	
		clearWindow();

		drawMeshes();


		swapBuffer();
		pollEvents();
	}
	// we will extend this funciton to take in multiple inputs

	void drawMeshes() {
		mesh->draw();
	
	}
	void processInput() {

		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

	}


};