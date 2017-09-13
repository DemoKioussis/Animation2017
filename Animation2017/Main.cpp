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
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f
};
GLuint indices[6] = {  // note that we start from 0!
	0, 1, 3,  // first Triangle
	1, 2, 3   // second Triangle
};




WindowManager* windowManager;
Camera* camera;
int main()
{



	windowManager = new WindowManager(SCR_WIDTH, SCR_HEIGHT);

	Shader shaderProg("ShaderSources/vert.vs", "ShaderSources/frag.fs");

	Mesh* mesh = new Mesh();

	camera = new Camera();


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
	glm::mat4 model, view, projection;
	unsigned int viewLoc = glGetUniformLocation(shaderProg.ID, "view");
	unsigned int modelLoc = glGetUniformLocation(shaderProg.ID, "model");
	unsigned int projLoc = glGetUniformLocation(shaderProg.ID, "projection");

	while (windowManager->windowHasClosed())
	{

		projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		view = glm::translate(view, camera->Position);

		model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));

		//model = glm::mat4x4(0);
		// get matrix's uniform location and set matrix
		shaderProg.use();

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));



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



