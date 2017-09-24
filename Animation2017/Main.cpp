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
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const int framerate = 120;

// to load into vectors

GLfloat vertices [] = {
	0,0,0
	,-20.000000,-20.000000,0.000000
	,20.000000,-20.000000,0.000000
	,0.001999,0.001999,39.998001
	,-20.000000,20.000000,0.000000
	,20.000000,20.000000,0.000000


};
GLuint indices[] = {  // note that we start from 0!

	1,2,3
	,4,2,1
	,1,3,4
	,2,5,3
	,5,4,3
	,5,2,4


};




WindowManager* windowManager;
Camera* camera;
int main()
{



	windowManager = new WindowManager(SCR_WIDTH, SCR_HEIGHT);

	Shader shaderProg("ShaderSources/vert.vs", "ShaderSources/frag.fs");

	Mesh* mesh = new Mesh();

	camera = new Camera();
	camera->translate(glm::vec3(0, 0, -50));

	InputManager::setWindow(windowManager);
	InputManager::setCamera(camera);
	InputManager::initialize();


	std::vector<GLfloat> verts(0);
	std::vector<GLuint> inds(0);

	for (int i = 0; i < sizeof(vertices)/sizeof(GLfloat); i++) {
		verts.push_back(vertices[i]);
	}
	for (int i = 0; i < sizeof(indices)/sizeof(GLuint); i++) {
		inds.push_back(indices[i]);

	}


	mesh->setVerticies(&verts);
	mesh->setIndices(&inds);
	mesh->setShader(&shaderProg);


	TimeSystem::begin();
	windowManager->setFramerate(framerate);
	windowManager->mesh = mesh;
	glm::mat4 model(1.0f), projection;
	unsigned int viewLoc = glGetUniformLocation(shaderProg.ID, "view");
	unsigned int modelLoc = glGetUniformLocation(shaderProg.ID, "model");
	unsigned int projLoc = glGetUniformLocation(shaderProg.ID, "projection");

	projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 10000.0f);

	while (windowManager->windowHasClosed())
	{
		
		glm::mat4 view;
		view = glm::translate(view, camera->Position);

		model = glm::rotate(model, 0.00001f, glm::vec3(0.0f, 1.0f, 1.0f));


		shaderProg.use();

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//glfwSetCursorPos(windowManager->getWindow(), 333, 333);

		TimeSystem::update();
		InputManager::processInput();

	//	physicsManager->physicsTick();
		


		shaderProg.setVec4("ourColor", glm::vec4(1, 1, 1, 1));
		windowManager->frameTick();

	}


	glfwTerminate();
	delete(windowManager);
	return 0;
}



