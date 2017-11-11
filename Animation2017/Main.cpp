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
#pragma endregion


	int numX = 1;
	int numY = 1;
	int numZ = 1;

	glCullFace(GL_BACK);

	float disp = 1.0f;
	//std::vector<Entity*> entities(0);

	float fieldSize = 25.f;

	/*for (int x = -numX / 2; x < numX / 2; x++) {
		for (int y = -numY / 2; y < numY / 2; y++) {
			for (int z = -numZ / 2; z < numZ / 2; z++) {*/
	/*for (int x = 0; x < numX; x++) {
		for (int y = 0; y < numY;y++) {
			for (int z = 0; z < numZ;z++) {

				float randomX = ((float(rand()) / float(RAND_MAX)) - 0.5f) * fieldSize;
				float randomY = ((float(rand()) / float(RAND_MAX)) - 0.5f) * fieldSize;
				float randomZ = ((float(rand()) / float(RAND_MAX)) - 0.5f) * fieldSize;

				Entity* e = new Entity(false);
				RenderComponent *r = new RenderComponent();
				r->setMeshID((x + y + z) % 3);
				PhysicsComponent* p = new PhysicsComponent();
				CollisionComponent* c = new CollisionComponent();
				c->setMeshID((x + y + z) % 3);
				e->addComponent(p);
				e->addComponent(r);
				e->addComponent(c);
				glm::mat4 moment(1.0f);
				moment*(1.0f / 12.0f) * (2.0f);
				RenderEngine::getInstance()->addComponent(r);
				PhysicsEngine::getInstance()->addComponent(p);
				e->translation = glm::translate(e->translation, glm::vec3(randomX, randomY, randomZ));
				CollisionEngine::getInstance()->addComponent(c);
				entities.push_back(e);
			}
		}
	}

	CollisionEngine::getInstance()->calculateUniqueIndices(); // Important for updating the info about the collisions
	CollisionEngine::getInstance()->updateAllBoundingBoxes(); // Can only be called after calculating the unique indices

	InputManager::Entities = &entities;*/
	

	SceneLoading::getInstance()->loadScene("scene_1");

	glm::mat4 rotation(1.0f), projection;

	GLuint projLoc = glGetUniformLocation(shaderProg.ID, "projection");
	GLuint viewLoc = glGetUniformLocation(shaderProg.ID, "view");
	GLuint modelLoc = glGetUniformLocation(shaderProg.ID, "model");

	projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
	shaderProg.use();

	shaderProg.setMat4(projLoc, projection);
	shaderProg.setVec3("lightDirection", glm::vec3(0, -1, 1));
	shaderProg.setVec3("ambientLight", glm::vec3(1.0f, 1.0f, 1.0f));
	float cosT = 0, sinT = 0;

	 

#pragma region mainLoop
	while (windowManager->windowHasClosed())
	{
		float cosT = cosf(glfwGetTime()), sinT = sinf(glfwGetTime());
		glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 model = glm::rotate(glm::mat4(1.0f), (float)glfwGetTime(), glm::vec3(1,1,1));
		shaderProg.setMat4(viewLoc, view);
	//	shaderProg.setMat4(modelLoc, model);

		TimeSystem::update();				
		
		PhysicsEngine::getInstance()->step();
		
		windowManager->frameTick();
	}

#pragma endregion

	glfwTerminate();
	delete(windowManager);
	return 0;
}





