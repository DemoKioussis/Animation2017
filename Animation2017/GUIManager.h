#pragma once

#include <nanogui/nanogui.h>
#include <iostream>
#include <Eigen/Core>
#include "PhysicsBuilder.h"

#include "Entity.h"
#include "RenderComponent.h"

#include "PhysicsEngine.h"
#include "PhysicsComponent.h"

#include "CollisionEngine.h"
#include "CollisionComponent.h"

#include "SceneLoading.h"
#include "EntityManager.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

using namespace nanogui;


class GUIManager
{


public:
	static void Initialize();
	void interactWithGUI();
	void instantiateGameObject(glm::vec3 col, int shape);

	void createMainMenuWindow();
	void createEditorWindow();
	void createInspectorWindow();
	void createInstantiationWindow();

	void createTransformPopUpWindow();

	void updateButtons();
	void updateCurrentEntity();
	void modifyCurrentEntity();

	static GUIManager* getInstance();

private:
	static GUIManager* instance;
	GUIManager();


	//GUI INSPECTOR
	bool isRenderComponentOn = true;
	bool isPhysicsComponentOn = true;
	bool isCollisionComponentOn = true;

	double gameObjectPosX = 0;
	double gameObjectPosY = 0;
	double gameObjectPosZ = 0;

	double gameObjectPosOldX = 0;
	double gameObjectPosOldY = 0;
	double gameObjectPosOldZ = 0;

	double gameObjectRotX = 0;
	double gameObjectRotY = 0;
	double gameObjectRotZ = 0;


	double gameObjectScaleX = 1;
	double gameObjectScaleY = 1;
	double gameObjectScaleZ = 1;

	double gameObjectScaleOldX = 1;
	double gameObjectScaleOldY = 1;
	double gameObjectScaleOldZ = 1;


	double gameObjectForceX = 0;
	double gameObjectForceY = 0;
	double gameObjectForceZ = 0;

	std::string savedSceneName = "Scene 0";
	std::string currentGameObjectName = "GameObject 0";
	std::string instantiatedGameObjectName = "New GameObject 0";

	float mass = 1;

	//Container
	Entity* eGameObject = nullptr;
	RenderComponent* rGameObject = nullptr;
	PhysicsComponent* pGameObject = nullptr;
	CollisionComponent* cGameObject = nullptr;

	//GUI
	CheckBox *cb;
	CheckBox *cb2;
	CheckBox *cb3;


	//POPUP TRANSFORM
	PopupButton *popupBtnTranslation;
	Popup *popupT;
	FloatBox<float>* translateBoxX;
	FloatBox<float>* translateBoxY;
	FloatBox<float>* translateBoxZ;

	FloatBox<float>* rotateBoxX;
	FloatBox<float>* rotateBoxY;
	FloatBox<float>* rotateBoxZ;

	FloatBox<float>* scaleBoxX;
	FloatBox<float>* scaleBoxY;
	FloatBox<float>* scaleBoxZ;


	//GUI INTERACTIONS
	bool isOnEdit = false;
	bool isNewScene = false;
	bool isLoadedScene = false;

	//GUI WINDOWS
	Window *gameObjectInspectorWindow;
	Window *gameObjectInstantiationWindow;
	Window *engineEditorWindow;
	Window *mainMenuWindow;

	//GUI I/O
	char* fileNameChar;
	string fileNameExt;
	string fileName;

	//INSTANTIATION
	glm::vec3 instantiatedColor = glm::vec3(1, 0, 0);
	int instantiatedShape = 0;



};