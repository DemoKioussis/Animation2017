#include "PhysicsSimulation.h"

PhysicsSimulation* PhysicsSimulation::instance = nullptr;
Screen* PhysicsSimulation::screen = nullptr;


void PhysicsSimulation::Initialize() {
	if (instance)
		return;
	PhysicsSimulation *physicsSimulation = new PhysicsSimulation();
	instance = physicsSimulation;

}

PhysicsSimulation::PhysicsSimulation()
{
	TimeSystem::begin();

	windowManager = new WindowManager(SCR_WIDTH, SCR_HEIGHT);
	windowManager->setFramerate(framerate);


	shaderProg = new Shader("ShaderSources/vert.vs", "ShaderSources/frag.fs");

	camera = new Camera(windowManager);
	camera->translate(glm::vec3(0, 0, -100));

	EntityManager::Initialize();

	InputManager::setWindow(windowManager);
	InputManager::setCamera(camera);
	InputManager::initialize();


	RenderEngine::Initialize();
	RenderEngine::getInstance()->setShader(shaderProg);

	PhysicsEngine::Initialize();
	PhysicsEngine::getInstance()->setGravity(glm::vec3(0, -1, 0), 0);

	CollisionEngine::Initialize();

	SceneLoading::Initialize();

	cubemapTexture = loadSkybox(faces);
	Mesh::cubemapTexture = cubemapTexture;

	int numX = 1;
	int numY = 1;
	int numZ = 1;

	glCullFace(GL_BACK);

	disp = 1.0f;

	fieldSize = 25.f;
	TimeSystem::resetTime();
	PhysicsEngine::getInstance()->disable();


	glm::mat4 rotation(1.0f), projection;

	projLoc = glGetUniformLocation(shaderProg->ID, "projection");
	viewLoc = glGetUniformLocation(shaderProg->ID, "view");
	modelLoc = glGetUniformLocation(shaderProg->ID, "model");
	Shader::isRenderSkyboxLoc = glGetUniformLocation(shaderProg->ID, "isRenderSkybox");
	projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
	shaderProg->use();

	shaderProg->setMat4(projLoc, projection);
	shaderProg->setVec3("lightDirection", glm::vec3(-1.4, -1.5, -2));
	shaderProg->setVec3("ambientLight", glm::vec3(1.0f, 1.0f, 1.0f));




	TimeSystem::resetTime();


	screen = new Screen();
	screen->initialize(windowManager->getWindow(), true);
	GUIManager::Initialize();



	//MOVE THESE
	glfwSetCursorPosCallback(windowManager->getWindow(),
		[](GLFWwindow *, double x, double y) {
		screen->cursorPosCallbackEvent(x, y);
	}
	);

	glfwSetMouseButtonCallback(windowManager->getWindow(),
		[](GLFWwindow *, int button, int action, int modifiers) {
		screen->mouseButtonCallbackEvent(button, action, modifiers);
	}
	);

	glfwSetKeyCallback(windowManager->getWindow(),
		[](GLFWwindow *, int key, int scancode, int action, int mods) {
		screen->keyCallbackEvent(key, scancode, action, mods);
	}
	);

	glfwSetCharCallback(windowManager->getWindow(),
		[](GLFWwindow *, unsigned int codepoint) {
		screen->charCallbackEvent(codepoint);
	}
	);

	glfwSetDropCallback(windowManager->getWindow(),
		[](GLFWwindow *, int count, const char **filenames) {
		screen->dropCallbackEvent(count, filenames);
	}
	);

	glfwSetScrollCallback(windowManager->getWindow(),
		[](GLFWwindow *, double x, double y) {
		screen->scrollCallbackEvent(x, y);
	}
	);

	glfwSetFramebufferSizeCallback(windowManager->getWindow(),
		[](GLFWwindow *, int width, int height) {
		screen->resizeCallbackEvent(width, height);
	}
	);



}



PhysicsSimulation* PhysicsSimulation::getInstance() {
	return instance;
}

Camera* PhysicsSimulation::getCamera()
{
	return camera;
}



WindowManager * PhysicsSimulation::getWindowManager()
{
	return windowManager;
}

Shader*  PhysicsSimulation::getShader()
{
	return shaderProg;
}

GLuint PhysicsSimulation::getView()
{
	return viewLoc;
}

Screen* PhysicsSimulation::getScreen()
{
	return screen;
}

bool PhysicsSimulation::shouldWindowClose()
{
	return windowManager->windowHasClosed();
}
