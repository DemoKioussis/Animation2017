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
		// verticies	/				/color					//normals
		//back
		-modelScale,-modelScale,-modelScale,		0.8f,  0.3, 0.3,		 0,0,-1 		//back left
		,modelScale,-modelScale,-modelScale,		0.80f, 0.3, 0.3,		 0,0,-1		//back right
		,modelScale,modelScale,-modelScale,			0.80f, 0.3, 0.3,		 0,0,-1		//front right
		,-modelScale,modelScale,-modelScale,		0.80f, 0.3, 0.3,		 0,0,-1		//front left

		//front
		,-modelScale,-modelScale,modelScale,		0.80f, 0.3, 0.3,	 0,0,1 		//back left
		,modelScale,-modelScale,modelScale,			0.80f, 0.3, 0.3,		 0,0,1		//back right
		,modelScale,modelScale,modelScale,			0.80f, 0.3, 0.3,		 0,0,1		//front right
		,-modelScale,modelScale,modelScale,			0.80f, 0.3, 0.3,		 0,0,1		//front left

		//left
		,-modelScale,-modelScale,-modelScale,		0.50f, 0.3, 0.3,		 -1,0,0 		//back left
		,-modelScale,-modelScale,modelScale,		0.50f, 0.3, 0.3,		 -1,0,0		//back right
		,-modelScale,modelScale,modelScale,			0.50f, 0.3, 0.3,		 -1,0,0		//front right
		,-modelScale,modelScale,-modelScale,		0.50f, 0.3, 0.3,		 -1,0,0		//front left
		//right
		,modelScale,-modelScale,-modelScale,		0.30f, 0.8, 0.3,		 1,0,0 		//back left
		,modelScale,-modelScale,modelScale,			0.30f, 0.8, 0.3,		 1,0,0		//back right
		,modelScale,modelScale,modelScale,			0.30f, 0.8, 0.3,		 1,0,0		//front right
		,modelScale,modelScale,-modelScale,			0.30f, 0.8, 0.3,		 1,0,0		//front left

		//TOP
		,-modelScale,modelScale,-modelScale,		0.30f, 0.3, 0.8,		 0,1,0 		//back left
		,-modelScale,modelScale,modelScale,			0.30f, 0.3, 0.8,		 0,1,0		//back right
		,modelScale,modelScale,modelScale,			0.30f, 0.3, 0.8,		 0,1,0		//front right
		,modelScale,modelScale,-modelScale,			0.3f, 0.3, 0.8,		 0,1,0		//front left

		//BOTTOM
		,-modelScale,-modelScale,-modelScale,		0.50f, 0.5, 0.8,		 0,-1,0	//back left
		,-modelScale,-modelScale,modelScale,		0.50f, 0.5, 0.8,		 0,-1,0	//back right
		,modelScale,-modelScale,modelScale,			0.50f, 0.5, 0.8,		 0,-1,0	//front right
		,modelScale,-modelScale,-modelScale,		0.50f, 0.5, 0.8,		 0,-1,0	//front left

	};

	GLuint indices[] = {  // note that we start from 0!
		 0,1,2
		,2,3,0

		,4,5,6
		,6,7,4

		,8,9,10
		,10,11,8

		,12,13,14
		,14,15,12

		,16,17,18
		,18,19,16

		,20,21,22
		,22,23,20	

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

	CollisionEngine::Initialize();

#pragma endregion

	Mesh* mesh = makeMesh();
 	RenderEngine::getInstance()->addRenderReference(mesh);
	int numEnts = 2;//00;
	float disp = 2.1f;
	std::vector<Entity*> entities(0);
	for (int i = 0; i < numEnts;i++) {
		Entity* e = new Entity;
		RenderComponent *r = new RenderComponent();
		r->setMeshID(0);
		PhysicsComponent* p = new PhysicsComponent();
		CollisionComponent* c = new CollisionComponent(ColliderType::VERTICES);
		e->addComponent(p);
		e->addComponent(r);
		e->addComponent(c);
		RenderEngine::getInstance()->addComponent(r);
		PhysicsEngine::getInstance()->addComponent(p);
		e->translation = glm::translate(e->translation, glm::vec3(-1, 0, 0)*(i *disp));		
		CollisionEngine::getInstance()->addComponent(c);
		entities.push_back(e);
	}

	CollisionEngine::getInstance()->calculateUniqueIndicesAndFurthestDistances(); // Important for u[datign the info about the collisions

	InputManager::Entities = &entities;
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

		glm::mat4 view = camera->GetViewMatrix();

		shaderProg.setMat4(viewLoc, view);

		TimeSystem::update();
		InputManager::processInput();
		

		PhysicsEngine::getInstance()->step();
		CollisionEngine::getInstance()->step();
		
		windowManager->frameTick();
	}

#pragma endregion

	glfwTerminate();
	delete(windowManager);
	return 0;
}





