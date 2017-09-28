#include "InputManager.h"


Camera* InputManager::camera = nullptr;
WindowManager* InputManager::windowManager = nullptr;
GLboolean InputManager::firstMouse = false;
GLfloat InputManager::lastX = 0;
GLfloat InputManager::lastY = 0;

void InputManager::initialize() {
	if (windowManager != nullptr) {
		glfwMakeContextCurrent(windowManager->getWindow());
		glfwSetFramebufferSizeCallback(windowManager->getWindow(), framebuffer_size_callback);
		glfwSetCursorPosCallback(windowManager->getWindow(), mouse_callback);
		glfwSetScrollCallback(windowManager->getWindow(), scroll_callback);
		glfwSetKeyCallback(windowManager->getWindow(), key_callback);

	}

	else std::cout << "cannot initialize input manager - missing window reference" << std::endl;

}
void InputManager::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	windowManager->setDimensions(width, height);
	glViewport(0, 0, width, height);
}
void InputManager::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera->ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void InputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera->ProcessMouseScroll(yoffset);
}
void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_F1) == GLFW_PRESS) {
		camera->ToggleFpsMode();
	}
}

void InputManager::setCamera(Camera* c) {
	if (windowManager != nullptr) {
		camera = c;
		lastX = windowManager->width() / 2.0f;
		lastY = windowManager->height() /2.0f;
		firstMouse = true;
	}
	else std::cout << "cannot set camera for input manager - missing window reference" << std::endl;
}
void InputManager::setWindow(WindowManager* wm) {
	windowManager = wm;
}
void InputManager::processInput() {
	
	glfwPollEvents();
	if (windowManager->getWindow() != nullptr) {


		if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(windowManager->getWindow(), true);

		processCameraInput();
	}
	else
		std::cout << "Cannot process input - no window" << std::endl;
}

void InputManager::processCameraInput() {

	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
		camera->ProcessKeyboard(DIRECTION::FORWARD, TimeSystem::getFrameDeltaTime());
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
		camera->ProcessKeyboard(DIRECTION::BACKWARD, TimeSystem::getFrameDeltaTime());
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
		camera->ProcessKeyboard(DIRECTION::LEFT, TimeSystem::getFrameDeltaTime());
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
		camera->ProcessKeyboard(DIRECTION::RIGHT, TimeSystem::getFrameDeltaTime());
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera->ProcessKeyboard(DIRECTION::UP, TimeSystem::getFrameDeltaTime());
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera->ProcessKeyboard(DIRECTION::DOWN, TimeSystem::getFrameDeltaTime());
	}
}
