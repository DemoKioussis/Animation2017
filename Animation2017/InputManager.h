#pragma once

#include <glad\glad.h>
#include <vector>
class Camera;
class WindowManager;
class GLFWwindow;

class Entity;

class InputManager {
private:
	static Camera* camera;
	static WindowManager* windowManager;

	static GLfloat lastX;
	static GLfloat lastY;
	static GLboolean firstMouse;
public:
	static void initialize();

	static void setWindow(WindowManager* wm);
	static void setCamera(Camera* c);


	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

	static void processInput();

	static void processCameraInput();
	static std::vector<Entity *> *Entities;

};