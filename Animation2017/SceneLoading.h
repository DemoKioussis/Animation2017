#pragma once
#include "Mesh.h"
#include "RenderEngine.h"
#include "RenderComponent.h"
#include "PhysicsEngine.h"
#include "PhysicsComponent.h"
#include "CollisionEngine.h"
#include "CollisionComponent.h"
#include "InputManager.h"
#include "TimeSystem.h"
#include <GLFW\glfw3.h>

#include <fstream>
#include <iostream>
#include <string>
#include "splitstring.h"
#include <glm/gtc/matrix_transform.hpp>
class SceneLoading
{
	

public:
	void loadScene(string sceneName);
	 static void Initialize();
	 static SceneLoading* getInstance();

	//~SceneLoading();
private:
	 SceneLoading();
	static SceneLoading* instance;

	 Mesh* cube;
	 Mesh* cylinder;
	 Mesh* ellipsoid;
	 Mesh* sphere;
	 Mesh* sphereLR;
	 Mesh* sphereSky;

	 
	//Mesh* sphereHD 
};

