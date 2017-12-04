#include "InputManager.h"
#include "WindowManager.h"
#include "Camera.h"
#include "TimeSystem.h"
#include "PhysicsComponent.h"
#include "PhysicsEngine.h"
#include "Entity.h"
#include "SceneLoading.h"
#include "EntityManager.h"
#include "PhysicsSimulation.h"
#include "WindowRender.h"

Camera* InputManager::camera = nullptr;
WindowManager* InputManager::windowManager = nullptr;
GLboolean InputManager::firstMouse = false;
GLfloat InputManager::lastX = 0;
GLfloat InputManager::lastY = 0;
GLboolean InputManager::isInCameraMode = false;

void InputManager::initialize() {
	if (windowManager != nullptr) {
		glfwMakeContextCurrent(windowManager->getWindow());
		glfwSetFramebufferSizeCallback(windowManager->getWindow(), framebuffer_size_callback);
		glfwSetCursorPosCallback(windowManager->getWindow(), mouse_callback);
		glfwSetScrollCallback(windowManager->getWindow(), scroll_callback);
		glfwSetKeyCallback(windowManager->getWindow(), key_callback);
		glfwSetDropCallback(windowManager->getWindow(), drop_callback);
		glfwSetCharCallback(windowManager->getWindow(), char_callback);
		glfwSetMouseButtonCallback(windowManager->getWindow(), mousebutton_callback);
		
	}

	else std::cout << "cannot initialize input manager - missing window reference" << std::endl;

}
void InputManager::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	if (!isInCameraMode)
	{
		PhysicsSimulation::getInstance()->getScreen()->resizeCallbackEvent(width, height);
	}

	
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
	if (!isInCameraMode)
	{
		PhysicsSimulation::getInstance()->getScreen()->cursorPosCallbackEvent(xpos, ypos);
	}
	else
	{
		camera->ProcessMouseMovement(xoffset, yoffset);
	}

	
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void InputManager::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{

	if (!isInCameraMode)
	{
		PhysicsSimulation::getInstance()->getScreen()->resizeCallbackEvent(xoffset, yoffset);
	}
	else
	{
		camera->ProcessMouseScroll(yoffset);
		
	}
	
}
void InputManager::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	PhysicsSimulation::getInstance()->getScreen()->keyCallbackEvent(key, scancode, action, mods);

	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_F1) == GLFW_PRESS) {
		isInCameraMode = !isInCameraMode;
		camera->ToggleFpsMode();
	}

	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_P) == GLFW_PRESS) {
		if(PhysicsEngine::getInstance()->isEnabled())
			PhysicsEngine::getInstance()->disable();
		else {
			TimeSystem::resetTime();
			PhysicsEngine::getInstance()->enable();
		}
	}

	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_O) == GLFW_PRESS) {
		if (CollisionEngine::getInstance()->isEnabled())
			CollisionEngine::getInstance()->disable();
		else {
			CollisionEngine::getInstance()->enable();
		}
	}

	if ((glfwGetKey(windowManager->getWindow(), GLFW_KEY_R) == GLFW_PRESS)) {
		if (RenderEngine::getInstance()->isEnabled())
			RenderEngine::getInstance()->disable();
		else {
			RenderEngine::getInstance()->enable();
		}
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_I) == GLFW_PRESS) {
		std::cout << "Delta time: " << TimeSystem::getPhysicsDeltaTime() << std::endl;
	}

	if ((glfwGetKey(windowManager->getWindow(), GLFW_KEY_K) == GLFW_PRESS) ) {
		std::vector<Entity*> *Entities;
		Entities= EntityManager::getInstance()->getEntities();
		for (int i = 0; i < Entities->size();i++) {
			auto e = (Entities->at(i));
			PhysicsComponent *p = (PhysicsComponent*)e->getComponent(PHYSICS_COMPONENT);
			if (p != nullptr) {
				p->setAngularMomentum(glm::vec3());
				p->setMomentum(glm::vec3());
			}

		}
	}
	if ((glfwGetKey(windowManager->getWindow(), GLFW_KEY_B) == GLFW_PRESS)) {
		std::vector<Entity*> *Entities;
		Entities = EntityManager::getInstance()->getEntities();
		for (int i = 0; i < Entities->size();i++) {
			auto e = (Entities->at(i));
			PhysicsComponent *p = (PhysicsComponent*)e->getComponent(PHYSICS_COMPONENT);
			if (p != nullptr) {
				glm::vec3 randForce((float)rand() / RAND_MAX, (float)rand() / RAND_MAX, (float) rand() / RAND_MAX);
				PhysicsEngine::getInstance()->addForce(p, randForce *20000.0f,glm::vec3(0));
				p->setMomentum(glm::vec3());
			}

		}
	}



	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
		camera->ToggleSpeed();
		
	}

	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_UP) == GLFW_PRESS) {
		PhysicsEngine::getInstance()->setGravity(glm::vec3(0, 1, 0), 9.81);
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_DOWN) == GLFW_PRESS) {
		PhysicsEngine::getInstance()->setGravity(glm::vec3(0, -1, 0), 9.81);
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS) {
		PhysicsEngine::getInstance()->setGravity(glm::vec3(-1, 0, 0), 9.81);
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_LEFT) == GLFW_PRESS) {
		PhysicsEngine::getInstance()->setGravity(glm::vec3(1, 0, 0), 9.81);
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_PAGE_UP) == GLFW_PRESS) {
		PhysicsEngine::getInstance()->setGravity(glm::vec3(0, 0, 1), 9.81);
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_PAGE_DOWN) == GLFW_PRESS) {
		PhysicsEngine::getInstance()->setGravity(glm::vec3(0, 0, -1), 9.81);
	}
}

void InputManager::drop_callback(GLFWwindow *, int count, const char ** filenames)
{
	if (!isInCameraMode)
	{
		PhysicsSimulation::getInstance()->getScreen()->dropCallbackEvent(count, filenames);
	}

}

void InputManager::char_callback(GLFWwindow *, unsigned int codepoint)
{
	if (!isInCameraMode)
	{
		PhysicsSimulation::getInstance()->getScreen()->charCallbackEvent(codepoint);
	}
}

void InputManager::mousebutton_callback(GLFWwindow *, int button, int action, int modifiers)
{
	if (!isInCameraMode)
	{
		PhysicsSimulation::getInstance()->getScreen()->mouseButtonCallbackEvent(button, action, modifiers);
	}
}

bool InputManager::getMode()
{
	return isInCameraMode;
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

	float force = 100.0f;
	glm::vec3 x(1, 0, 0);
	glm::vec3 y(0, 1, 0);
	glm::vec3 z(0, 0, 1);


	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_KP_5) == GLFW_PRESS) {
		std::vector<Entity*> *Entities;
		Entities = EntityManager::getInstance()->getEntities();
		for (int i = 0; i < Entities->size();i++) {
			auto e = (Entities->at(i));
			PhysicsComponent *p = (PhysicsComponent*)e->getComponent(PHYSICS_COMPONENT);
			if(p!=nullptr)
				PhysicsEngine::getInstance()->addForce(p, -z*force*(float)(1), y);

		}
	}
	if ((glfwGetKey(windowManager->getWindow(), GLFW_KEY_KP_8) == GLFW_PRESS) || (glfwGetKey(windowManager->getWindow(), GLFW_KEY_8) == GLFW_PRESS)) {
		std::vector<Entity*> *Entities;
		Entities = EntityManager::getInstance()->getEntities();
		for (int i = 0; i < Entities->size();i++) {
			auto e = (Entities->at(i));
			PhysicsComponent *p = (PhysicsComponent*)e->getComponent(PHYSICS_COMPONENT);
			if (p != nullptr)
				PhysicsEngine::getInstance()->addForce(p, z*force*(float)(1), y);

		}
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_KP_4) == GLFW_PRESS) {
		std::vector<Entity*> *Entities;
		Entities = EntityManager::getInstance()->getEntities();
		for (int i = 0; i < Entities->size();i++) {
			auto e = (Entities->at(i));
			PhysicsComponent *p = (PhysicsComponent*)e->getComponent(PHYSICS_COMPONENT);
			if (p != nullptr)
				PhysicsEngine::getInstance()->addForce(p, x*force*(float)( 1), y);
		}
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_KP_6) == GLFW_PRESS) {
		std::vector<Entity*> *Entities;
		Entities = EntityManager::getInstance()->getEntities();
		for (int i = 0; i < Entities->size();i++) {
			auto e = (Entities->at(i));
			PhysicsComponent *p = (PhysicsComponent*)e->getComponent(PHYSICS_COMPONENT);
			if (p != nullptr)
				PhysicsEngine::getInstance()->addForce(p, -x*force*(float)( 1), y);
		}
	}

	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_KP_9) == GLFW_PRESS) {
		std::vector<Entity*> *Entities;
		Entities = EntityManager::getInstance()->getEntities();
		for (int i = 0; i < Entities->size();i++) {
			auto e = (Entities->at(i));
			PhysicsComponent *p = (PhysicsComponent*)e->getComponent(PHYSICS_COMPONENT);
			if (p != nullptr)
				PhysicsEngine::getInstance()->addForce(p, x*force*(float)(1), z);

		}
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_KP_7) == GLFW_PRESS) {
		std::vector<Entity*> *Entities;
		Entities = EntityManager::getInstance()->getEntities();
		for (int i = 0; i < Entities->size();i++) {
			auto e = (Entities->at(i));
			PhysicsComponent *p = (PhysicsComponent*)e->getComponent(PHYSICS_COMPONENT);
			if (p != nullptr)
				PhysicsEngine::getInstance()->addForce(p, -x*force*(float)( 1), z);

		}
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_KP_0) == GLFW_PRESS) {
		TimeSystem::setTimeScale(TimeSystem::getTimeScale() - 0.01f);
	}
	if (glfwGetKey(windowManager->getWindow(), GLFW_KEY_KP_2) == GLFW_PRESS) {
		TimeSystem::setTimeScale(TimeSystem::getTimeScale() + 0.01f);
	}

}
void InputManager::updateSkybox() {
	glm::mat4 viewMat = camera->GetViewMatrix();
	viewMat[0][0] = 1;
	viewMat[1][1] = 1;
	viewMat[2][2] = 1;
	viewMat[0][1] = 0;
	viewMat[0][2] = 0;
	viewMat[1][0] = 0;
	viewMat[1][2] = 0;
	viewMat[2][0] = 0;
	viewMat[2][1] = 0;
	std::vector<Entity*> *Entities;
	Entities = EntityManager::getInstance()->getEntities();
	Entities->at(0)->translation = viewMat;
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

