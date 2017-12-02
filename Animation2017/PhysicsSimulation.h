#pragma once
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
#include "EntityManager.h"
#include "Skybox.h"

#include "GUIManager.h"


class PhysicsSimulation
{

public:
	static void Initialize();
	static PhysicsSimulation* getInstance();
	Camera* getCamera();
	WindowManager* getWindowManager();
	Shader* getShader();
	GLuint getView();
	static Screen* getScreen();
	bool shouldWindowClose();
	



private:
	static Screen *screen;
	static PhysicsSimulation* instance;
	PhysicsSimulation();
	WindowManager* windowManager;
	
	unsigned int cubemapTexture;

	float disp = 1.0f;

	float fieldSize = 25.f;

	//SCREEN SIZE
	const unsigned int SCR_WIDTH = 1600;
	const unsigned int SCR_HEIGHT = 900;

	//FRAMERATE
	const int framerate = 120;
	
	Camera* camera;
	float modelScale = 1.0f;

	vector<std::string> faces =
	{
		"Skybox/TropicalSunnyDayRight2048.png",
		"Skybox/TropicalSunnyDayLeft2048.png",
		"Skybox/TropicalSunnyDayUp2048.png",
		"Skybox/TropicalSunnyDayDown2048.png",
		"Skybox/TropicalSunnyDayBack2048.png",
		"Skybox/TropicalSunnyDayFront2048.png"
	};

	Shader* shaderProg;
	float cosT = 0, sinT = 0;

	GLuint projLoc;
	GLuint viewLoc;
	GLuint modelLoc;



	bool isRenderComponentOn = true;
	bool isPhysicsComponentOn = true;

	double gameObjectPosX = 0;
	double gameObjectPosY = 0;
	double gameObjectPosZ = 0;

	double gameObjectRotX = 0;
	double gameObjectRotY = 0;
	double gameObjectRotZ = 0;

	double gameObjectScaleX = 0;
	double gameObjectScaleY = 0;
	double gameObjectScaleZ = 0;

	double gameObjectForceX = 0;
	double gameObjectForceY = 0;
	double gameObjectForceZ = 0;



	std::string savedSceneName = "Scene 0";
	std::string currentGameObjectName = "GameObject 0";
	std::string instantiatedGameObjectName = "New GameObject 0";

	Color* colval = new Color(0.5f, 0.5f, 0.7f, 1.f);



	bool isOnEdit = false;
	bool isNewScene = false;
	bool isLoadedScene = false;

	//GUI WINDOWS
	Window *gameObjectInspectorWindow;
	Window *gameObjectInstantiationWindow;
	Window *engineEditorWindow;
	Window *mainMenuWindow;


	char* fileNameChar;
	string fileNameExt;
	string fileName;

	





};

