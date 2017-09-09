#include "GLSetup.h"
#include "WindowManager.h"
#include "Shader.h"
#include "Mesh.h"
#include <iostream>



/**
This class right now just sets thigns up, ideally by the end of this it would be a list of settings and nothing else
*/
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// to load into vectors
GLfloat vertices[12] = {
	0.5f,  0.5f, 0.0f,  // top right
	0.5f, -0.5f, 0.0f,  // bottom right
	-0.5f, -0.5f, 0.0f,  // bottom left
	-0.5f,  0.5f, 0.0f   // top left 
};
GLuint indices[6] = {  // note that we start from 0!
	0, 1, 3,  // first Triangle
	1, 2, 3   // second Triangle
};

WindowManager* windowManager;
int main()
{

	windowManager = new WindowManager();
	windowManager->setWindow(GLSetup::makeWindow(SCR_WIDTH, SCR_HEIGHT));
	windowManager->setBackgroundColor(glm::vec4(0.5, 0.5, 0.5, 1));
	Shader shaderProg("ShaderSources/vert.vs", "ShaderSources/frag.fs");

	Mesh* mesh = new Mesh();



	std::vector<GLfloat> verts(0);
	std::vector<GLuint> inds(0);

	for (int i = 0; i < 12; i++) {
		verts.push_back(vertices[i]);
		std::cout << verts[i] << std::endl;
	}
	for (int i = 0; i < 6; i++) {
		inds.push_back(indices[i]);
		std::cout << inds[i] << std::endl;

	}


	mesh->setVerticies(&verts);
	mesh->setIndices(&inds);
	mesh->setShader(&shaderProg);


	while (windowManager->windowHasClosed())
	{
		//physicsManager->updatePhysics()
		float timeValue = glfwGetTime();
		float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

		shaderProg.setVec4("ourColor", glm::vec4(0, greenValue, 0, 1));

		windowManager->clearWindow();

		mesh->draw();


		windowManager->swapBuffer();
		windowManager->pollEvents();

	}


	glfwTerminate();
	delete(windowManager);
	return 0;
}



