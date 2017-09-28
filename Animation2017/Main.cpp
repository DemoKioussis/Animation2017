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

/**
This class right now just sets thigns up, ideally by the end of this it would be a list of settings and nothing else
*/
// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

const int framerate = 60;


WindowManager* windowManager;
Camera* camera;

void programLoop() {


}

int main()
{

#pragma region initRegion
	TimeSystem::begin();

	windowManager = new WindowManager(SCR_WIDTH, SCR_HEIGHT);
	windowManager->setFramerate(framerate);

	Shader shaderProg("ShaderSources/vert.vs", "ShaderSources/frag.fs");

	camera = new Camera(windowManager);
	camera->translate(glm::vec3(0, 0, -50));

	InputManager::setWindow(windowManager);
	InputManager::setCamera(camera);
	InputManager::initialize();
#pragma endregion

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

	Mesh* mesh = new Mesh();
	std::vector<GLfloat> positions(0), colors(0), normals(0);
	std::vector<GLuint> inds(0);
	for (int i = 0; i < sizeof(vertices) / sizeof(GLfloat); i+=9) {
		positions.push_back(vertices[i]);
		positions.push_back(vertices[i+1]);
		positions.push_back(vertices[i+2]);

		colors.push_back(vertices[i+3]);
		colors.push_back(vertices[i+4]);
		colors.push_back(vertices[i+5]);

		normals.push_back(vertices[i+6]);
		normals.push_back(vertices[i+7]);
		normals.push_back(vertices[i+8]);
	}
	for (int i = 0; i < sizeof(indices) / sizeof(GLuint); i++) {
		inds.push_back(indices[i]);
	}

	mesh->setVerticies(positions,colors,normals);
	mesh->setIndices(&inds);
	mesh->setShader(&shaderProg);
#pragma endregion

	RenderEngine::addRenderTarget(*mesh);

	glm::mat4 model(1.0f), projection;
	glm::vec3 lightDir(1, 1, 1);

	GLuint projLoc = glGetUniformLocation(shaderProg.ID, "projection");
	GLuint viewLoc = glGetUniformLocation(shaderProg.ID, "view");
	GLuint modelLoc = glGetUniformLocation(shaderProg.ID, "model");


	projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);


	



	shaderProg.use();

	shaderProg.setMat4(projLoc, projection);
	shaderProg.setVec3("lightDirection", glm::vec3(1, 1, 0));
	shaderProg.setVec3("ambientLight", glm::vec3(0.5f, 1.0f, 1.0f));

#pragma region mainLoop
	while (windowManager->windowHasClosed())
	{

		glm::mat4 view = camera->GetViewMatrix();
		model = glm::rotate(model, 0.001f*TimeSystem::getFrameDeltaTime(), glm::vec3(0.0f, 0.0f, 1));

		shaderProg.setMat4(modelLoc,model);
		shaderProg.setMat4(viewLoc, view);

		//programLoop();
		//for now keep stuff in here so its a bit clearer
		TimeSystem::update();
		InputManager::processInput();

		//	physicsManager->physicsTick();

		windowManager->frameTick();
	}

#pragma endregion

	glfwTerminate();
	delete(windowManager);
	return 0;
}





