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


WindowManager* windowManager;
Camera* camera;

Mesh* makeMesh() {

#pragma region MeshStuff

	// to load into vectors


	GLfloat vertices[] = {
		// verticies	//color					//normals
		-50,-50,0,		1.0f, 0.5, 0.3,		-2,-2,-3		//back left
		,50,-50,0,		1.0f, 0.5, 0.3,		 2,-2,-3		//back right
		,50,50,0,		1.0f, 0.5, 0.3,		 2, 2,-3		//front right
		,-50,50,0,		1.0f, 0.5, 0.3,		-2, 2,-3		//front left
		,0,0,100,		1.0f, 0.5, 0.3,		 0, 0, 1		//top middle
	};

	GLuint indices[] = {  // note that we start from 0!
		0,1,2
		,0,3,2
		,0,4,1
		,1,4,2
		,2,4,3
		,3,4,0
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
	camera->translate(glm::vec3(0, 0, -200));

	InputManager::setWindow(windowManager);
	InputManager::setCamera(camera);
	InputManager::initialize();

	RenderEngine::Initialize();
	RenderEngine::getInstance()->setShader(&shaderProg);

	PhysicsEngine::Initialize();
	PhysicsEngine::getInstance()->setGravity(glm::vec3(0, -1, 0), 0.000000981f);

#pragma endregion

	Mesh* mesh = makeMesh();
 	RenderEngine::getInstance()->addRenderReference(mesh);

	RenderComponent *render1 = new RenderComponent();
	RenderComponent *render2 = new RenderComponent();
	render1->setMeshID(0);
	render2->setMeshID(0);

	PhysicsComponent *physics = new PhysicsComponent();

	Entity* entity1 = new Entity();
	entity1->addComponent(render1);
	entity1->addComponent(physics);

	Entity* entity2 = new Entity();
	entity2->addComponent(render2);

	

	RenderEngine::getInstance()->addComponent(render1);
	RenderEngine::getInstance()->addComponent(render2);
	PhysicsEngine::getInstance()->addComponent(physics);

	PhysicsEngine::getInstance()->addForce(physics, glm::vec3(0, 0.03, 0.03f), glm::vec3());
	glm::mat4 rotation(1.0f), projection;
	glm::vec3 lightDir(1, 1, 1);

	GLuint projLoc = glGetUniformLocation(shaderProg.ID, "projection");
	GLuint viewLoc = glGetUniformLocation(shaderProg.ID, "view");
	GLuint modelLoc = glGetUniformLocation(shaderProg.ID, "model");

	projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);


	shaderProg.use();

	shaderProg.setMat4(projLoc, projection);
	shaderProg.setVec3("lightDirection", glm::vec3(1, 1, 0));
	shaderProg.setVec3("ambientLight", glm::vec3(0.5f, 1.0f, 1.0f));

#pragma region mainLoop
	while (windowManager->windowHasClosed())
	{

		glm::mat4 view = camera->GetViewMatrix();
		rotation = glm::rotate(rotation, 0.001f*TimeSystem::getFrameDeltaTime(), glm::vec3(0.0f, 0.0f, 1));
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), glm::vec3(100, 0, 0));
	//	entity1->transform = translation*rotation;
		entity2->transform = glm::inverse(translation)*glm::inverse(rotation);

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





