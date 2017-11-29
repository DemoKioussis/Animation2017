#include "WindowManager.h"
#include "Shader.h"
#include "Mesh.h"
#include "Camera.h"
#include "InputManager.h"
#include <iostream>
#include "TimeSystem.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Entity.h"
#include "RenderComponent.h"

#include "PhysicsEngine.h"
#include "PhysicsComponent.h"

#include "CollisionEngine.h"
#include "CollisionComponent.h"

#include "SceneLoading.h"

#include "Skybox.h"
/**
This class right now just sets thigns up, ideally by the end of this it would be a list of settings and nothing else
*/
// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

const int framerate = 120;

WindowManager* windowManager;
Camera* camera;

float modelScale = 1.0f;

int main()
{

#pragma region initRegion
	TimeSystem::begin();

	windowManager = new WindowManager(SCR_WIDTH, SCR_HEIGHT);
	windowManager->setFramerate(framerate);

	Shader shaderProg("ShaderSources/vert.vs", "ShaderSources/frag.fs");

	camera = new Camera(windowManager);
	camera->translate(glm::vec3(0, 0, -100));

	InputManager::setWindow(windowManager);
	InputManager::setCamera(camera);
	InputManager::initialize();

	

	RenderEngine::Initialize();
	RenderEngine::getInstance()->setShader(&shaderProg);

	PhysicsEngine::Initialize();
	PhysicsEngine::getInstance()->setGravity(glm::vec3(0, -1, 0),0);

	CollisionEngine::Initialize();

	SceneLoading::Initialize();

	vector<std::string> faces=
	{
			"Skybox/TropicalSunnyDayRight2048.png",
			"Skybox/TropicalSunnyDayLeft2048.png",
			"Skybox/TropicalSunnyDayUp2048.png",
			"Skybox/TropicalSunnyDayDown2048.png",
			"Skybox/TropicalSunnyDayBack2048.png",
			"Skybox/TropicalSunnyDayFront2048.png"
	};
	unsigned int cubemapTexture = loadSkybox(faces);
	Mesh::cubemapTexture = cubemapTexture;

#pragma endregion


	int numX = 1;
	int numY = 1;
	int numZ = 1;

	glCullFace(GL_BACK);

	float disp = 1.0f;

	float fieldSize = 25.f;
	TimeSystem::resetTime();
	PhysicsEngine::getInstance()->disable();
	SceneLoading::getInstance()->loadScene("bowl");
	//std::cout << "test1 "<< camera->skybox->getID() << std::endl;

	glm::mat4 rotation(1.0f), projection;

	GLuint projLoc = glGetUniformLocation(shaderProg.ID, "projection");
	GLuint viewLoc = glGetUniformLocation(shaderProg.ID, "view");
	GLuint modelLoc = glGetUniformLocation(shaderProg.ID, "model");
	Shader::isRenderSkyboxLoc = glGetUniformLocation(shaderProg.ID, "isRenderSkybox");
	projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
	shaderProg.use();

	shaderProg.setMat4(projLoc, projection);
	shaderProg.setVec3("lightDirection", glm::vec3(-1.4, -1.5, -2));
	shaderProg.setVec3("ambientLight", glm::vec3(1.0f, 1.0f, 1.0f));
	float cosT = 0, sinT = 0;
	TimeSystem::resetTime();
	//RenderComponent* r = (RenderComponent*)InputManager::Entities->at(1)->getComponent(RENDER_COMPONENT);
	//RenderComponent* r2 = (RenderComponent*)InputManager::Entities->at(2)->getComponent(RENDER_COMPONENT);

	//r->setColor(glm::vec3(1, 0, 0));
	//r2->setColor(glm::vec3(0, 0, 1));

	//RenderEngine::getInstance()->updateColors();

#pragma region mainLoop
	while (windowManager->windowHasClosed())
	{
		
		float cosT = cosf(glfwGetTime()), sinT = sinf(glfwGetTime());
		glm::mat4 view = camera->GetViewMatrix();
		shaderProg.setMat4(viewLoc, view);
		
		TimeSystem::update();				

		PhysicsEngine::getInstance()->step();
		
		windowManager->frameTick();
	}

#pragma endregion

	glfwTerminate();
	delete(windowManager);
	return 0;
}





