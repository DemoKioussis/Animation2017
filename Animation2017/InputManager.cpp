#include "InputManager.h"
#include "WindowManager.h"
#include "Camera.h"
#include "TimeSystem.h"
#include "PhysicsComponent.h"
#include "PhysicsEngine.h"
#include "Entity.h"
#include "SceneLoading.h"

Camera* InputManager::camera = nullptr;
WindowManager* InputManager::windowManager = nullptr;
GLboolean InputManager::firstMouse = false;
GLfloat InputManager::lastX = 0;
GLfloat InputManager::lastY = 0;
std::vector<Entity*> *InputManager::Entities =new std::vector<Entity*>();

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

	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_P) == GLFW_PRESS) {
//		TimeSystem::setTimeScale(abs(1 - TimeSystem::getTimeScale()));
		if(PhysicsEngine::getInstance()->isEnabled())
			PhysicsEngine::getInstance()->disable();
		else
			PhysicsEngine::getInstance()->enable();

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
	float force = 0.010f;
	glm::vec3 x(10, 0, 0);
	glm::vec3 y(0, 10, 0);
	glm::vec3 z(0, 0, 10);
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_KP_5) == GLFW_PRESS) {
		for (int i = 0; i < Entities->size();i++) {
			auto e = (Entities->at(i));
			PhysicsComponent *p = (PhysicsComponent*)e->getComponent(PHYSICS_COMPONENT);
			PhysicsEngine::getInstance()->addForce(p, -z*force*(float)(1), y);

		}
	}
	if ((glfwGetKey(windowManager->getWindow(), GLFW_KEY_KP_8) == GLFW_PRESS) || (glfwGetKey(windowManager->getWindow(), GLFW_KEY_8) == GLFW_PRESS)) {
		for (int i = 0; i < Entities->size();i++) {
			auto e = (Entities->at(i));
			PhysicsComponent *p = (PhysicsComponent*)e->getComponent(PHYSICS_COMPONENT);
			PhysicsEngine::getInstance()->addForce(p, z*force*(float)(1), y);

		}
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_KP_4) == GLFW_PRESS) {
		for (int i = 0; i < Entities->size();i++) {
			auto e = (Entities->at(i));
			PhysicsComponent *p = (PhysicsComponent*)e->getComponent(PHYSICS_COMPONENT);
			PhysicsEngine::getInstance()->addForce(p, x*force*(float)( 1), y);
		}
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_KP_6) == GLFW_PRESS) {
		for (int i = 0; i < Entities->size();i++) {
			auto e = (Entities->at(i));
			PhysicsComponent *p = (PhysicsComponent*)e->getComponent(PHYSICS_COMPONENT);
			PhysicsEngine::getInstance()->addForce(p, -x*force*(float)( 1), y);
		}
	}

	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_KP_9) == GLFW_PRESS) {
		for (int i = 0; i < Entities->size();i++) {
			auto e = (Entities->at(i));
			PhysicsComponent *p = (PhysicsComponent*)e->getComponent(PHYSICS_COMPONENT);
			PhysicsEngine::getInstance()->addForce(p, x*force*(float)(1), z);

		}
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_KP_7) == GLFW_PRESS) {
		for (int i = 0; i < Entities->size();i++) {
			auto e = (Entities->at(i));
			PhysicsComponent *p = (PhysicsComponent*)e->getComponent(PHYSICS_COMPONENT);
			PhysicsEngine::getInstance()->addForce(p, -x*force*(float)( 1), z);

		}
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_L) == GLFW_PRESS) {
		//load
		std::cout << "Enter scene name you want to load (no extensions)" << std::endl;
		char* sceneName= new char[100];
		std::cin >> sceneName;
		SceneLoading::getInstance()->loadScene(sceneName);

	}
}

void InputManager::processCameraInput() {
	float deltaT = TimeSystem::getFrameDeltaTime();
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_W) == GLFW_PRESS) {
		camera->ProcessKeyboard(DIRECTION::FORWARD, deltaT);
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_S) == GLFW_PRESS) {
		camera->ProcessKeyboard(DIRECTION::BACKWARD, deltaT);
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_A) == GLFW_PRESS) {
		camera->ProcessKeyboard(DIRECTION::LEFT, deltaT);
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_D) == GLFW_PRESS) {
		camera->ProcessKeyboard(DIRECTION::RIGHT, deltaT);
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera->ProcessKeyboard(DIRECTION::UP, deltaT);
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		camera->ProcessKeyboard(DIRECTION::DOWN, deltaT);
	}

}

