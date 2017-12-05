#pragma once

#include <nanogui/nanogui.h>
#include <iostream>
#include <fstream>
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

#include "nanovg/nanovg_gl.h"
#include "nanovg/perf.h"



using namespace nanogui;


class GUIManager
{


public:
	static void Initialize();
	void interactWithGUI();
	void instantiateGameObject(glm::vec3 col, int shape);
	void initializeEmptyGameObject();

	void createMainMenuWindow();
	void createEditorWindow();
	void createInspectorWindow();
	void createInstantiationWindow();

	void createTransformPopUpWindow();
	void createPhysicsPopUpWindow();
	void createGravityPopUpWindow();

	void updateButtons();
	void updateCurrentEntity();
	void modifyCurrentEntity();

	void selectedGameObject();
	void instantiateDot();
	void showDot();

	
	static GUIManager* getInstance();

private:
	static GUIManager* instance;
	GUIManager();

	//GUI INSPECTOR VALUES
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


	double gameObjectGravityX = 0;
	double gameObjectGravityY = -1;
	double gameObjectGravityZ = 0;

	double gameObjectForceX = 0;
	double gameObjectForceY = 0;
	double gameObjectForceZ = 0;

	double gameObjectTorqueX = 0;
	double gameObjectTorqueY = 0;
	double gameObjectTorqueZ = 0;

	double gameObjectMass = 1;
	bool isGameObjectStatic = false;
	bool isGravityOn = false;
	double gameObjectBounciness = 0.1f;

	int iteratorGameObject = 0;

	string tempfile = "newtons_cradle";

	//Container
	Entity* eGameObject = nullptr;
	RenderComponent* rGameObject = nullptr;
	PhysicsComponent* pGameObject = nullptr;
	CollisionComponent* cGameObject = nullptr;

	//Container for dot
	Entity* eGameObjectC = nullptr;
	RenderComponent* rGameObjectC = nullptr;

	//TempColor
	glm::vec3 colorEdit = glm::vec3(-1,-1,-1);
	


	//GUI
	CheckBox *cb;
	CheckBox *cb2;
	CheckBox *cb3;
	CheckBox *cbG;

	//TIME
	FloatBox<float>* timeScaleBox;

	//ITERATOR
	IntBox<int>* iteratorBox;


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

	//POPUP GRAVITY
	PopupButton *popupBtnGravity;
	Popup *popupG;

	FloatBox<float>*  gravityBoxX;
	FloatBox<float>*  gravityBoxY;
	FloatBox<float>*  gravityBoxZ;



	//POPUP PHYSICS
	PopupButton *popupBtnForce;
	Popup *popupF;


	FloatBox<float>* forceBoxX;
	FloatBox<float>* forceBoxY;
	FloatBox<float>* forceBoxZ;

	FloatBox<float>* torqueBoxX;
	FloatBox<float>* torqueBoxY;
	FloatBox<float>* torqueBoxZ;



	CheckBox *staticCheckBox;
	FloatBox<float>* massBox;
	FloatBox<float>* bouncinessBox;


	//GUI INTERACTIONS
	bool isOnEdit = false;
	bool isNewScene = false;
	bool isLoadedScene = false;

	//GUI BUTTONS
	Button *bEditorPlay;
	Button *bEditorEdit;
	Button *bEditorOpen;
	Button *bEditorSave;
	Button *bEditorReload;

	//GUI WINDOWS
	Window *gameObjectInspectorWindow;
	Window *gameObjectInstantiationWindow;
	Window *engineEditorWindow;
	Window *mainMenuWindow;

	//GUI I/O
	string fileNameExt;
	string fileName;
	string  savedFileName;

	//INSTANTIATION
	glm::vec3 instantiatedColor = glm::vec3(1, 0, 0);
	int instantiatedShape = 0;



};