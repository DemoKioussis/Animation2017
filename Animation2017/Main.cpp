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
/**
This class right now just sets thigns up, ideally by the end of this it would be a list of settings and nothing else
*/
// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

const int framerate = 60;
const int physicsrate = 120;

WindowManager* windowManager;
Camera* camera;

float modelScale = 1.0f;
Mesh* makeMesh() {

#pragma region MeshStuff

	// to load into vectors


	GLfloat vertices[] = {
		// verticies	//color					//normals
		-modelScale,-modelScale,0,		1.0f, 0.5, 0.3,		-2,-2,-3		//back left
		,modelScale,-modelScale,0,		1.0f, 0.5, 0.3,		 2,-2,-3		//back right
		,modelScale,modelScale,0,		1.0f, 0.5, 0.3,		 2, 2,-3		//front right
		,-modelScale,modelScale,0,		1.0f, 0.5, 0.3,		-2, 2,-3		//front left
		,0,0,2* modelScale,				1.0f, 0.5, 0.3,		 0, 0, 1		//top middle

		,-modelScale,-modelScale,0,		1.0f, 0.5, 0.3,		 0,0,1 		//back left
		,modelScale,-modelScale,0,		1.0f, 0.5, 0.3,		 0,0,1		//back right
		,modelScale,modelScale,0,		1.0f, 0.5, 0.3,		 0,0,1		//front right
		,-modelScale,modelScale,0,		1.0f, 0.5, 0.3,		 0,0,1		//front left

	};

	GLuint indices[] = {  // note that we start from 0!
		 0,4,1
		,1,4,2
		,2,4,3
		,3,4,0

		,5,6,7
		,5,8,7
	};

	std::vector<GLfloat> positions(0), colors(0), normals(0);
	std::vector<GLuint>* inds= new std::vector<GLuint>(0);
	for (int i = 0; i < sizeof(vertices) / sizeof(GLfloat); i += 9) {
		positions.push_back(vertices[i]);
		positions.push_back(vertices[i + 1]);
		positions.push_back(vertices[i + 2]);

		colors.push_back(vertices[i + 3]);
		colors.push_back(vertices[i + 4]);
		colors.push_back(vertices[i + 5]);

		normals.push_back(vertices[i + 6]);
		normals.push_back(vertices[i + 7]);
		normals.push_back(vertices[i + 8]);
	}
	for (int i = 0; i < sizeof(indices) / sizeof(GLuint); i++) {
		inds->push_back(indices[i]);
	}

	Mesh*m = new Mesh();
	m->setVerticies(positions, colors, normals);
	m->setIndices(inds);
	return m;
}
int main()
{

#pragma region initRegion
	TimeSystem::begin();

	windowManager = new WindowManager(SCR_WIDTH, SCR_HEIGHT);
	windowManager->setFramerate(framerate);

	Shader shaderProg("ShaderSources/vert.vs", "ShaderSources/frag.fs");

	camera = new Camera(windowManager);
	camera->translate(glm::vec3(0, 0, -10));

	InputManager::setWindow(windowManager);
	InputManager::setCamera(camera);
	InputManager::initialize();

	RenderEngine::Initialize();
	RenderEngine::getInstance()->setShader(&shaderProg);

	PhysicsEngine::Initialize();
	PhysicsEngine::getInstance()->setGravity(glm::vec3(0, -1, 0),0);

#pragma endregion

	Mesh* mesh = makeMesh();
 	RenderEngine::getInstance()->addRenderReference(mesh);
	int numEnts = 100;
	std::vector<Entity*> entities(0);
	for (int i = 0; i < numEnts;i++) {
		Entity* e = new Entity;
		RenderComponent *r = new RenderComponent();
		r->setMeshID(0);
		PhysicsComponent* p = new PhysicsComponent();
		e->addComponent(p);
		e->addComponent(r);
		RenderEngine::getInstance()->addComponent(r);
		PhysicsEngine::getInstance()->addComponent(p);
		e->translation = glm::translate(e->translation, glm::vec3(1, 0, 0)*(i * 5.0f));
		entities.push_back(e);
		p->addAngularVelocity(glm::vec3(0, 0, 1), i*0.01);
		PhysicsEngine::getInstance()->addForce(p, glm::vec3(0, 0, i*i), glm::vec3());
	}

	InputManager::physics = (PhysicsComponent*)entities[0]->getComponent(PHYSICS_COMPONENT);
	glm::mat4 rotation(1.0f), projection;

	GLuint projLoc = glGetUniformLocation(shaderProg.ID, "projection");
	GLuint viewLoc = glGetUniformLocation(shaderProg.ID, "view");
	GLuint modelLoc = glGetUniformLocation(shaderProg.ID, "model");

	projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
	shaderProg.use();

	shaderProg.setMat4(projLoc, projection);
	shaderProg.setVec3("lightDirection", glm::vec3(0, 1, 0));
	shaderProg.setVec3("ambientLight", glm::vec3(1.0f, 1.0f, 1.0f));
	float cosT = 0, sinT = 0;

#pragma region mainLoop
	while (windowManager->windowHasClosed())
	{

		glm::mat4 view = camera->GetViewMatrix();

		shaderProg.setMat4(viewLoc, view);

		TimeSystem::update();
		InputManager::processInput();
		

		PhysicsEngine::getInstance()->step();

		
		windowManager->frameTick();
	}

#pragma endregion

	glfwTerminate();
	delete(windowManager);
	return 0;
}





