#include "WindowRender.h"
#include"WindowManager.h"
#include"Camera.h"

WindowRender* WindowRender::instance = nullptr;

void WindowRender::Initialize()
{
	if (instance)
		return;
	WindowRender * windowRender = new WindowRender();

	instance = windowRender;
}


WindowRender* WindowRender::getInstance() {
	return instance;
}

void WindowRender::Render()
{
	float cosT = cosf(glfwGetTime()), sinT = sinf(glfwGetTime());
	glm::mat4 view = PhysicsSimulation::getInstance()->getCamera()->GetViewMatrix();
	PhysicsSimulation::getInstance()->getShader()->setMat4(PhysicsSimulation::getInstance()->getView(), view);

	TimeSystem::update();

	PhysicsEngine::getInstance()->step();

	GUIManager::getInstance()->interactWithGUI();


	if (TimeSystem::frameCheck() >= 1.0f / 60.0f) {
		glEnable(GL_DEPTH_TEST);
		InputManager::processInput();
		PhysicsSimulation::getInstance()->getWindowManager()->clearWindow();
		PhysicsSimulation::getInstance()->getShader()->use();
		PhysicsSimulation::getInstance()->getWindowManager()->drawMeshes();
	   
		if (!InputManager::getMode())
		{
			PhysicsSimulation::getInstance()->getScreen()->drawContents();
			PhysicsSimulation::getInstance()->getScreen()->drawWidgets();
		}
		
		PhysicsSimulation::getInstance()->getWindowManager()->swapBuffer();
		glDisable(GL_DEPTH_TEST);
		TimeSystem::frameStep();
	}

}


