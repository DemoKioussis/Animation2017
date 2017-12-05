#include "GUIManager.h"
#include "PhysicsSimulation.h"


GUIManager* GUIManager::instance = nullptr;

void GUIManager::Initialize() {
	if (instance)
		return;
	GUIManager *guiManager = new GUIManager();
	instance = guiManager;

}



void GUIManager::createMainMenuWindow()
{
	//MAIN MENU WINDOW
	mainMenuWindow = new Window(PhysicsSimulation::getScreen(), "Main Menu");
	mainMenuWindow->center();
	mainMenuWindow->setLayout(new GroupLayout());
	Button *bMainOpen = new Button(mainMenuWindow, "Open Existing Scene", ENTYPO_ICON_ARCHIVE);
	bMainOpen->setCallback([&] {
		 fileNameExt = file_dialog({ { "scene", "Scene Engine Parser File" } }, false);

		SceneLoading::getInstance()->loadScene(fileNameExt);


		//updateCurrentEntity();

		if (fileNameExt != "")
		{
			selectedGameObject();	updateCurrentEntity();	updateButtons();  instantiateDot(); showDot();
			isNewScene = true;

			isOnEdit = true;
		}


	});
	Button *bMainNew = new Button(mainMenuWindow, "Create New Scene", ENTYPO_ICON_BRUSH);
	bMainNew->setCallback([&] {isNewScene = true;	 isOnEdit = true;	//PhysicsEngine::getInstance()->setGravity(glm::vec3(0.0f, -1.0f,0.0f), 9.8f);
	});


}



void GUIManager::createEditorWindow()
{
	initializeEmptyGameObject();

	engineEditorWindow = new Window(PhysicsSimulation::getInstance()->getScreen(), "Engine Editor");
	engineEditorWindow->setPosition((Eigen::Vector2i(10, 10)));
	engineEditorWindow->setLayout(new GroupLayout());
	engineEditorWindow->setTooltip("Allows user to edit scene, play out the scene and save the scene");
	bEditorPlay = new Button(engineEditorWindow, "Play", ENTYPO_ICON_CONTROLLER_PLAY);
	bEditorPlay->setFlags(Button::RadioButton);
	bEditorPlay->setCallback([&] {
		isOnEdit = false;

		TimeSystem::resetTime();
	});
	bEditorEdit = new Button(engineEditorWindow, "Edit", ENTYPO_ICON_EDIT);
	bEditorEdit->setFlags(Button::RadioButton);
	bEditorEdit->setPushed(true);
	bEditorEdit->setCallback([&] {
		isOnEdit = true;

	});
	bEditorOpen = new Button(engineEditorWindow, "Open Another Scene", ENTYPO_ICON_ARCHIVE);
	bEditorOpen->setCallback([&] {
		fileNameExt = file_dialog({ { "scene", "Scene Engine Parser File" } }, false);
		SceneLoading::getInstance()->loadScene(fileNameExt);
	

		
		//CHANGE ME
		
		if (fileNameExt != "")
		{
			bEditorPlay->setPushed(false);
			bEditorEdit->setPushed(true);

			iteratorGameObject = 0;	selectedGameObject();	updateCurrentEntity();	updateButtons();  instantiateDot(); showDot();
			isNewScene = true;

			isOnEdit = true;
		}

	
		
	});
	bEditorSave = new Button(engineEditorWindow, "Save", ENTYPO_ICON_SAVE);
	//THIS RETURNS THE FILE DESTINATION OF WHERE I DECIDED TO PUT THE FILE
	bEditorSave->setCallback([&] {
		 savedFileName = file_dialog(
		{ { "scene", "Scene Engine Parser File" } }, true);

		if (savedFileName != "")
		{
			MessageDialog *dlg = new MessageDialog(PhysicsSimulation::getScreen(), MessageDialog::Type::Information, "File", "Your scene has been successfully saved.");
			SceneLoading::getInstance()->saveScene(savedFileName);
		}
		else
		{
			MessageDialog *dlg = new MessageDialog(PhysicsSimulation::getScreen(), MessageDialog::Type::Warning, "File", "Your scene has not been saved!");
		}


	});
	bEditorReload = new Button(engineEditorWindow, "Reload Scene Completely", ENTYPO_ICON_CW);

	bEditorReload->setCallback([&] {
		
		SceneLoading::getInstance()->loadScene(fileNameExt);  
		if (fileNameExt != "")
		{
			iteratorGameObject = 0;  selectedGameObject(); updateCurrentEntity();  updateButtons();  instantiateDot(); showDot();
		}

	});



	new Label(engineEditorWindow, "Time Scale: ");
	timeScaleBox = new FloatBox<float>(engineEditorWindow);
	timeScaleBox->setEditable(true);
	timeScaleBox->setFixedSize(Vector2i(100, 20));
	timeScaleBox->setValue(1);
	timeScaleBox->setDefaultValue("1");
	timeScaleBox->setFontSize(16);
	timeScaleBox->setSpinnable(true);
	timeScaleBox->setMinValue(0);
	timeScaleBox->setFormat("[0-9]*\\.?[0-9]+");
	timeScaleBox->setValueIncrement(.05);
	timeScaleBox->setMaxValue(100);
	timeScaleBox->setCallback([&](float t) {  TimeSystem::setTimeScale(t); });




	engineEditorWindow->setVisible(false);
}

void GUIManager::createInstantiationWindow()
{
	gameObjectInstantiationWindow = new Window(PhysicsSimulation::getScreen(), "GameObject Instantiation");
	gameObjectInstantiationWindow->setPosition((Eigen::Vector2i(1400, 10)));
	gameObjectInstantiationWindow->setLayout(new BoxLayout(Orientation::Vertical,
		Alignment::Middle, 15, 6));
	new Label(gameObjectInstantiationWindow, "GameObject Shape ", "sans");
	ComboBox *cobo =
		new ComboBox(gameObjectInstantiationWindow, { "Cube", "Cylinder","Ellipsoid","Sphere" });
	cobo->setCallback([&](int shape) {instantiatedShape = shape; });
	cobo->setSide(Popup::Side::Left);
	cobo->setFontSize(16);
	cobo->setFixedSize(Vector2i(100, 20));

	new Label(gameObjectInstantiationWindow, "GameObject Color ", "sans");
	auto cp = new ColorPicker(gameObjectInstantiationWindow, { 255, 0, 0, 255 });
	cp->setFixedSize({ 100, 30 });
	cp->setSide(Popup::Side::Left);
	cp->setFinalCallback([&](const Color &c) {
		instantiatedColor.x = c.r();
		instantiatedColor.y = c.g();
		instantiatedColor.z = c.b();

	});
	//SET UP CALL BACK TO ASSOCIATE COLOR WITH INSTANTIATION
	new Label(gameObjectInstantiationWindow, "Instantiate GameObject ", "sans-bold");
	Button *b = gameObjectInstantiationWindow->add<Button>("Instantiate", ENTYPO_ICON_ROCKET);
	b->setBackgroundColor(Color(0, 100, 255, 25));
	b->setCallback([&] { instantiateGameObject(instantiatedColor, instantiatedShape); updateCurrentEntity();  updateButtons(); });
	b->setTooltip("This creates a gameobject with the specified shape and color specified.");

	gameObjectInstantiationWindow->setVisible(false);

}

void GUIManager::createTransformPopUpWindow()
{
	popupBtnTranslation = new PopupButton(gameObjectInspectorWindow, "Transform", ENTYPO_ICON_EXPORT);
	popupT = popupBtnTranslation->popup();
	popupT->setLayout(new GridLayout(Orientation::Horizontal, 6,
		Alignment::Middle, 15, 6));
	popupT->setTooltip("Change the Position, Rotation and Scale of the current GameObject.");

	

	new Label(popupT, " Translate X");
	translateBoxX = new FloatBox<float>(popupT);
	translateBoxX->setEditable(true);
	translateBoxX->setFixedSize(Vector2i(100, 20));
	translateBoxX->setValue(0);
	translateBoxX->setDefaultValue("0");
	translateBoxX->setFontSize(16);
	translateBoxX->setSpinnable(true);
	translateBoxX->setValueIncrement(.5);
	translateBoxX->setCallback([&](float x) {  gameObjectPosX = x; modifyCurrentEntity(); });


	new Label(popupT, "Translate Y");
	translateBoxY = new FloatBox<float>(popupT);
	translateBoxY->setEditable(true);
	translateBoxY->setFixedSize(Vector2i(100, 20));
	translateBoxY->setValue(0);
	translateBoxY->setDefaultValue("0");
	translateBoxY->setFontSize(16);
	translateBoxY->setSpinnable(true);
	translateBoxY->setValueIncrement(.5);
	translateBoxY->setCallback([&](float y) {  gameObjectPosY = y; modifyCurrentEntity(); });

	new Label(popupT, "Translate Z");
	translateBoxZ = new FloatBox<float>(popupT);
	translateBoxZ->setEditable(true);
	translateBoxZ->setFixedSize(Vector2i(100, 20));
	translateBoxZ->setValue(0);
	translateBoxZ->setDefaultValue("0");
	translateBoxZ->setFontSize(16);
	translateBoxZ->setSpinnable(true);
	translateBoxZ->setValueIncrement(.5);
	translateBoxZ->setCallback([&](float z) {  gameObjectPosZ = z; modifyCurrentEntity(); });


	

	new Label(popupT, " Rotate X");
	rotateBoxX = new FloatBox<float>(popupT);
	rotateBoxX->setEditable(true);
	rotateBoxX->setFixedSize(Vector2i(100, 20));
	rotateBoxX->setValue(0);
	rotateBoxX->setDefaultValue("0");
	rotateBoxX->setFontSize(16);
	rotateBoxX->setSpinnable(true);
	rotateBoxX->setValueIncrement(1);
	rotateBoxX->setCallback([&](float x) {  gameObjectRotX = x; modifyCurrentEntity(); });

	new Label(popupT, "Rotate Y");
	rotateBoxY = new FloatBox<float>(popupT);
	rotateBoxY->setEditable(true);
	rotateBoxY->setFixedSize(Vector2i(100, 20));
	rotateBoxY->setValue(0);
	rotateBoxY->setDefaultValue("0");
	rotateBoxY->setFontSize(16);
	rotateBoxY->setSpinnable(true);
	rotateBoxY->setValueIncrement(1);
	rotateBoxY->setCallback([&](float y) {  gameObjectRotY = y; modifyCurrentEntity(); });

	new Label(popupT, "Rotate Z");
	rotateBoxZ = new FloatBox<float>(popupT);
	rotateBoxZ->setEditable(true);
	rotateBoxZ->setFixedSize(Vector2i(100, 20));
	rotateBoxZ->setValue(0);
	rotateBoxZ->setDefaultValue("0");
	rotateBoxZ->setFontSize(16);
	rotateBoxZ->setSpinnable(true);
	rotateBoxZ->setValueIncrement(1);
	rotateBoxZ->setCallback([&](float z) {  gameObjectRotZ = z; modifyCurrentEntity(); });


	new Label(popupT, "Scale X");
	scaleBoxX = new FloatBox<float>(popupT);
	scaleBoxX->setEditable(true);
	scaleBoxX->setFixedSize(Vector2i(100, 20));
	scaleBoxX->setValue(1);
	scaleBoxX->setDefaultValue("1");
	scaleBoxX->setFontSize(16);
	scaleBoxX->setSpinnable(true);
	scaleBoxX->setMinValue(0.1);
	scaleBoxX->setFormat("[0-9]*\\.?[0-9]+");
	scaleBoxX->setValueIncrement(.1);
	scaleBoxX->setCallback([&](float x) {  if (x <= 0.1)x = .1; gameObjectScaleX = x; modifyCurrentEntity(); });

	new Label(popupT, "Scale Y");
	scaleBoxY = new FloatBox<float>(popupT);
	scaleBoxY->setEditable(true);
	scaleBoxY->setFixedSize(Vector2i(100, 20));
	scaleBoxY->setValue(1);
	scaleBoxY->setDefaultValue("1");
	scaleBoxY->setFontSize(16);
	scaleBoxY->setSpinnable(true);
	scaleBoxY->setMinValue(0.1);
	scaleBoxY->setFormat("[0-9]*\\.?[0-9]+");
	scaleBoxY->setValueIncrement(.1);
	scaleBoxY->setCallback([&](float y) {   if (y <= 0.1)y = .1;  gameObjectScaleY = y; modifyCurrentEntity(); });

	new Label(popupT, "Scale Z");
	scaleBoxZ = new FloatBox<float>(popupT);
	scaleBoxZ->setEditable(true);
	scaleBoxZ->setFixedSize(Vector2i(100, 20));
	scaleBoxZ->setValue(1);
	scaleBoxZ->setDefaultValue("1");
	scaleBoxZ->setFontSize(16);
	scaleBoxZ->setSpinnable(true);
	scaleBoxZ->setMinValue(0.1);
	scaleBoxZ->setFormat("[0-9]*\\.?[0-9]+");
	scaleBoxZ->setValueIncrement(.1);
	scaleBoxZ->setCallback([&](float z) {  if (z <= 0.1)z = .1; gameObjectScaleZ = z; modifyCurrentEntity(); });

}

void GUIManager::createPhysicsPopUpWindow()
{
	popupBtnForce = new PopupButton(gameObjectInspectorWindow, "Force", ENTYPO_ICON_EXPORT);
	popupF = popupBtnForce->popup();
	popupF->setLayout(new GridLayout(Orientation::Horizontal, 6,
		Alignment::Middle, 15, 6));
	popupF->setTooltip("Change the Force of the current GameObject.");


	new Label(popupF, " Apply Force X");
	forceBoxX = new FloatBox<float>(popupF);
	forceBoxX->setEditable(true);
	forceBoxX->setFixedSize(Vector2i(100, 20));
	forceBoxX->setValue(0);
	forceBoxX->setDefaultValue("0");
	forceBoxX->setFontSize(16);
	forceBoxX->setSpinnable(true);
	forceBoxX->setValueIncrement(.5);
	forceBoxX->setCallback([&](float x) {  gameObjectForceX = x; modifyCurrentEntity(); });

	new Label(popupF, " Apply Force  Y");
	forceBoxY = new FloatBox<float>(popupF);
	forceBoxY->setEditable(true);
	forceBoxY->setFixedSize(Vector2i(100, 20));
	forceBoxY->setValue(0);
	forceBoxY->setDefaultValue("0");
	forceBoxY->setFontSize(16);
	forceBoxY->setSpinnable(true);
	forceBoxY->setValueIncrement(.5);
	forceBoxY->setCallback([&](float y) {  gameObjectForceY = y; modifyCurrentEntity(); });

	new Label(popupF, " Apply Force  Z");
	forceBoxZ = new FloatBox<float>(popupF);
	forceBoxZ->setEditable(true);
	forceBoxZ->setFixedSize(Vector2i(100, 20));
	forceBoxZ->setValue(0);
	forceBoxZ->setDefaultValue("0");
	forceBoxZ->setFontSize(16);
	forceBoxZ->setSpinnable(true);
	forceBoxZ->setValueIncrement(.5);
	forceBoxZ->setCallback([&](float z) {  gameObjectForceZ = z; modifyCurrentEntity(); });

	new Label(popupF, " Apply Torque X");
	torqueBoxX = new FloatBox<float>(popupF);
	torqueBoxX->setEditable(true);
	torqueBoxX->setFixedSize(Vector2i(100, 20));
	torqueBoxX->setValue(0);
	torqueBoxX->setDefaultValue("0");
	torqueBoxX->setFontSize(16);
	torqueBoxX->setSpinnable(true);
	torqueBoxX->setValueIncrement(.5);
	torqueBoxX->setCallback([&](float x) {  gameObjectTorqueX = x; modifyCurrentEntity(); });

	new Label(popupF, " Apply Torque Y");
	torqueBoxY = new FloatBox<float>(popupF);
	torqueBoxY->setEditable(true);
	torqueBoxY->setFixedSize(Vector2i(100, 20));
	torqueBoxY->setValue(0);
	torqueBoxY->setDefaultValue("0");
	torqueBoxY->setFontSize(16);
	torqueBoxY->setSpinnable(true);
	torqueBoxY->setValueIncrement(.5);
	torqueBoxY->setCallback([&](float y) {  gameObjectTorqueY = y; modifyCurrentEntity(); });

	new Label(popupF, " Apply Torque Z");
	torqueBoxZ = new FloatBox<float>(popupF);
	torqueBoxZ->setEditable(true);
	torqueBoxZ->setFixedSize(Vector2i(100, 20));
	torqueBoxZ->setValue(0);
	torqueBoxZ->setDefaultValue("0");
	torqueBoxZ->setFontSize(16);
	torqueBoxZ->setSpinnable(true);
	torqueBoxZ->setValueIncrement(.5);
	torqueBoxZ->setCallback([&](float z) {  gameObjectTorqueZ = z; modifyCurrentEntity(); });



	new Label(popupF, "Mass");
	massBox = new FloatBox<float>(popupF);
	massBox->setEditable(true);
	massBox->setFixedSize(Vector2i(100, 20));
	massBox->setValue(0);
	massBox->setDefaultValue("0");
	massBox->setFontSize(16);
	massBox->setSpinnable(true);
	massBox->setMinValue(0);
	massBox->setFormat("[0-9]*\\.?[0-9]+");
	massBox->setValueIncrement(1);
	massBox->setCallback([&](float m) {  if (m <= 0.1)m = .1; gameObjectMass = m; modifyCurrentEntity(); });

	new Label(popupF, "Bounciness");
	bouncinessBox = new FloatBox<float>(popupF);
	bouncinessBox->setEditable(true);
	bouncinessBox->setFixedSize(Vector2i(100, 20));
	bouncinessBox->setValue(0.1);
	bouncinessBox->setDefaultValue("0");
	bouncinessBox->setFontSize(16);
	bouncinessBox->setSpinnable(true);
	bouncinessBox->setMinValue(0);
	bouncinessBox->setFormat("[0-9]*\\.?[0-9]+");
	bouncinessBox->setValueIncrement(.1);
	bouncinessBox->setCallback([&](float b) {  gameObjectBounciness = b; modifyCurrentEntity(); });

	staticCheckBox = new CheckBox(popupF, "Static");
	staticCheckBox->setFontSize(16);
	staticCheckBox->setChecked(false);
	staticCheckBox->setTooltip("Allow the GameObject to stay in place");
	staticCheckBox->setCallback([&](bool state)
	{
		isGameObjectStatic = state;
		modifyCurrentEntity();
	});



}

void GUIManager::createGravityPopUpWindow()
{

	cbG = new CheckBox(gameObjectInspectorWindow, "Gravity");
	cbG->setFontSize(16);
	cbG->setChecked(false);
	cbG->setTooltip("Turn on and off the gravity");
	cbG->setCallback([&](bool state)
	{
		isGravityOn = state;
		modifyCurrentEntity();
	});



	popupBtnGravity = new PopupButton(gameObjectInspectorWindow, "Gravity", ENTYPO_ICON_EXPORT);
	popupG = popupBtnGravity->popup();
	popupG->setLayout(new GridLayout(Orientation::Horizontal, 6,
		Alignment::Middle, 15, 6));
	popupG->setTooltip("Change the Gravity of the current scene.");


	new Label(popupG, " Apply Gravity X");
	gravityBoxX = new FloatBox<float>(popupG);
	gravityBoxX->setEditable(true);
	gravityBoxX->setFixedSize(Vector2i(100, 20));
	gravityBoxX->setValue(0);
	gravityBoxX->setDefaultValue("0");
	gravityBoxX->setFontSize(16);
	gravityBoxX->setSpinnable(true);
	gravityBoxX->setValueIncrement(.5);
	gravityBoxX->setCallback([&](float x) {  gameObjectGravityX = x; modifyCurrentEntity(); });

	new Label(popupG, " Apply Gravity Y");
	gravityBoxY = new FloatBox<float>(popupG);
	gravityBoxY->setEditable(true);
	gravityBoxY->setFixedSize(Vector2i(100, 20));
	gravityBoxY->setValue(0);
	gravityBoxY->setDefaultValue("0");
	gravityBoxY->setFontSize(16);
	gravityBoxY->setSpinnable(true);
	gravityBoxY->setValueIncrement(.5);
	gravityBoxY->setCallback([&](float y) {  gameObjectGravityY = y; modifyCurrentEntity(); });

	new Label(popupG, " Apply Gravity Z");
	gravityBoxZ = new FloatBox<float>(popupG);
	gravityBoxZ->setEditable(true);
	gravityBoxZ->setFixedSize(Vector2i(100, 20));
	gravityBoxZ->setValue(0);
	gravityBoxZ->setDefaultValue("0");
	gravityBoxZ->setFontSize(16);
	gravityBoxZ->setSpinnable(true);
	gravityBoxZ->setValueIncrement(.5);
	gravityBoxY->setCallback([&](float z) {  gameObjectGravityZ = z; modifyCurrentEntity(); });
	popupG->setVisible(false);
}


void GUIManager::createInspectorWindow()
{
	gameObjectInspectorWindow = new Window(PhysicsSimulation::getScreen(), "GameObject Inspector");
	gameObjectInspectorWindow->setPosition((Eigen::Vector2i(10, 300)));
	gameObjectInspectorWindow->setLayout(new GroupLayout());

	new Label(gameObjectInspectorWindow, "GameObject ID:");
	iteratorBox = new IntBox<int>(gameObjectInspectorWindow);
	iteratorBox->setEditable(true);
	iteratorBox->setFixedSize(Vector2i(100, 20));
	iteratorBox->setValue(0);
	iteratorBox->setDefaultValue("0");
	iteratorBox->setFontSize(16);
	iteratorBox->setSpinnable(true);
	iteratorBox->setMinValue(0);
	iteratorBox->setFormat("[0-9]*\\.?[0-9]+");
	iteratorBox->setValueIncrement(1);
	iteratorBox->setCallback([&](int i) {  if (EntityManager::getInstance()->getEntities()->size() != 0)
	{
		
		iteratorGameObject = i; selectedGameObject();	updateCurrentEntity();	updateButtons();
	}
	else
	{
		i = 0;
	}
		 });

	new Label(gameObjectInspectorWindow, "Render Properties", "sans");
	cb = new CheckBox(gameObjectInspectorWindow, "Render Component");
	cb->setFontSize(16);
	cb->setChecked(true);
	cb->setTooltip("Turn on and off the render component");
	//HOW TO GET THE STATE OF THE CHECK
	cb->setCallback([&](bool state)
	{
		isRenderComponentOn = state;
		modifyCurrentEntity();


	});

	createTransformPopUpWindow();


	new Label(gameObjectInspectorWindow, "Physics Properties", "sans");
	cb2 = new CheckBox(gameObjectInspectorWindow, "Physics Component");
	cb2->setFontSize(16);
	cb2->setChecked(true);
	cb2->setTooltip("Turn on and off the physics component.");
	cb2->setCallback([&](bool state)
	{
		isPhysicsComponentOn = state;
		modifyCurrentEntity();
	});



	createPhysicsPopUpWindow();

	createGravityPopUpWindow();



	new Label(gameObjectInspectorWindow, "Collision Properties", "sans");
	cb3 = new CheckBox(gameObjectInspectorWindow, "Collision Component");
	cb3->setFontSize(16);
	cb3->setChecked(true);
	cb3->setTooltip("Turn on and off the physics component.");

	cb3->setCallback([&](bool state)
	{
		isCollisionComponentOn = state;
		modifyCurrentEntity();


	});

	Button *b = gameObjectInspectorWindow->add<Button>("Next GameObject", ENTYPO_ICON_ARROW_RIGHT);
	b->setBackgroundColor(Color(0, 255, 0, 25));
	b->setCallback([&] {
		if (EntityManager::getInstance()->getEntities()->size() != 0)
		{
			iteratorGameObject++; selectedGameObject();	updateCurrentEntity();	updateButtons();
		}
		});
	b->setTooltip("Iterates to the next gameobject in the scene.");

	gameObjectInspectorWindow->setVisible(false);
	PhysicsSimulation::getScreen()->performLayout();

}


GUIManager::GUIManager()
{
	
	instantiateDot();
	createMainMenuWindow();
	createEditorWindow();
	createInstantiationWindow();
	createInspectorWindow();

}




void GUIManager::interactWithGUI()
{

	
	if (isOnEdit)
	{
		gameObjectInstantiationWindow->setVisible(true);
		gameObjectInspectorWindow->setVisible(true);
		PhysicsEngine::getInstance()->disable();
		popupBtnGravity->setVisible(isGravityOn);
		rGameObjectC->enable();
	}
	else if (!isOnEdit)
	{
		gameObjectInstantiationWindow->setVisible(false);
		gameObjectInspectorWindow->setVisible(false);

		PhysicsEngine::getInstance()->enable();
	
		rGameObjectC->disable();

	}

	if (isNewScene)
	{
		engineEditorWindow->setVisible(true);
		mainMenuWindow->setVisible(false);
	}



}

void GUIManager::instantiateGameObject(glm::vec3 col, int shape)
{
	//POSSIBLE LEAK BY ADDING THESE NEW ENTITIES
	eGameObject = new Entity();
	eGameObject->setShape(shape);
	eGameObject->setStatic(false);
	
	rGameObject = new RenderComponent();

	rGameObject->setColor(col);
	rGameObject->enable();
	rGameObject->setMeshID(shape);
	RenderEngine::getInstance()->addComponent(rGameObject);
	eGameObject->addComponent(rGameObject);



	
	pGameObject = new PhysicsComponent();
	pGameObject->enable();
	pGameObject->setMomentOfInertia(PhysicsBuilder::getMomentOfInertia(0, glm::mat4(), 1));
	pGameObject->setMass(1);
	pGameObject->setCoeffOfRestitution(gameObjectBounciness);
	pGameObject->setStatic(false);

	

	
	PhysicsEngine::getInstance()->addComponent(pGameObject);;
	eGameObject->addComponent(pGameObject);

	cGameObject = new CollisionComponent();
	cGameObject->enable();
	cGameObject->setMeshID(shape);
	CollisionEngine::getInstance()->addComponent(cGameObject);
	eGameObject->addComponent(cGameObject);
	CollisionEngine::getInstance()->updateAllBoundingBoxes();

	

	//Add To EntityManager;
	EntityManager::getInstance()->add(eGameObject);
	iteratorGameObject = EntityManager::getInstance()->getEntities()->size() -1;

	

}

void GUIManager::initializeEmptyGameObject()
{
	eGameObject = new Entity();
	rGameObject = new RenderComponent();
	pGameObject = new PhysicsComponent();
	cGameObject = new CollisionComponent();
}


void GUIManager::updateButtons()
{
	iteratorBox->setValue(iteratorGameObject);


	cb->setChecked(isRenderComponentOn);
	cb2->setChecked(isPhysicsComponentOn);
	cb3->setChecked(isCollisionComponentOn);

	translateBoxX->setValue(gameObjectPosX);
	translateBoxY->setValue(gameObjectPosY);
	translateBoxZ->setValue(gameObjectPosZ);

	rotateBoxX->setValue(gameObjectRotX);
	rotateBoxY->setValue(gameObjectRotY);
	rotateBoxZ->setValue(gameObjectRotZ);

	scaleBoxX->setValue(gameObjectScaleX);
	scaleBoxY->setValue(gameObjectScaleY);
	scaleBoxZ->setValue(gameObjectScaleZ);

	forceBoxX->setValue(gameObjectForceX);
	forceBoxY->setValue(gameObjectForceY);
	forceBoxZ->setValue(gameObjectForceZ);

	torqueBoxX->setValue(gameObjectTorqueX);
	torqueBoxY->setValue(gameObjectTorqueY);
	torqueBoxZ->setValue(gameObjectTorqueZ);

	staticCheckBox->setChecked(isGameObjectStatic);
	massBox->setValue(gameObjectMass);
	bouncinessBox->setValue(gameObjectBounciness);

	cbG->setChecked(isGravityOn);
	gravityBoxX->setValue(gameObjectGravityX);
	gravityBoxY->setValue(gameObjectGravityY);
	gravityBoxZ->setValue(gameObjectGravityZ);


}

void GUIManager::updateCurrentEntity()
{
	
	if (rGameObject == nullptr)
	{
		isRenderComponentOn = false;
	}
	else
	{
		isRenderComponentOn = rGameObject->isEnabled();
	}
	
	if (cGameObject == nullptr)
	{
		isCollisionComponentOn = false;
	}
	else
	{
		isCollisionComponentOn = cGameObject->isEnabled();
	}
	



	//CHANGE THESE TO THE CURENT GAMEOBJECT;

	gameObjectPosX = eGameObject->translation[3][0];
	gameObjectPosY = eGameObject->translation[3][1];
	gameObjectPosZ = eGameObject->translation[3][2];

	gameObjectPosOldX = eGameObject->translation[3][0];
	gameObjectPosOldY = eGameObject->translation[3][1];
	gameObjectPosOldZ = eGameObject->translation[3][2];

	glm::quat tmpQuat = glm::toQuat(eGameObject->rotation);
	glm::vec3 tmp = glm::eulerAngles(tmpQuat);
	gameObjectRotX = degrees(tmp.x);
	gameObjectRotY = degrees(-tmp.y);
	gameObjectRotZ = degrees(tmp.z);

	
	gameObjectScaleX = eGameObject->scale[0][0];
	gameObjectScaleY = eGameObject->scale[1][1];
	gameObjectScaleZ = eGameObject->scale[2][2];

	gameObjectScaleOldX = eGameObject->scale[0][0];
	gameObjectScaleOldY = eGameObject->scale[1][1];
	gameObjectScaleOldZ = eGameObject->scale[2][2];


	if (pGameObject == nullptr)
	{
		isPhysicsComponentOn = false;
	}
	else
	{
		isPhysicsComponentOn = pGameObject->isEnabled();
		glm::vec3 temp = pGameObject->getMomentum();
		gameObjectForceX = temp.x;
		gameObjectForceY = temp.y;
		gameObjectForceZ = temp.z;

		gameObjectBounciness = pGameObject->getCoeffOfRestitution();

		isGameObjectStatic = pGameObject->getIsStatic();

		gameObjectMass = pGameObject->getMass();

		//FOR TORQUE

		glm::vec3 tempT = pGameObject->getAngularMomentum();
		gameObjectTorqueX = tempT.x;
		gameObjectTorqueY = tempT.y;
		gameObjectTorqueZ = tempT.z;



	}


	if (PhysicsEngine::getInstance()->getGravity() != glm::vec3(0, 0, 0))
	{
		isGravityOn = true;
		glm::vec3 tempg = PhysicsEngine::getInstance()->getGravity();
		gameObjectGravityX = tempg.x / 9.8;
		gameObjectGravityY = tempg.y / 9.8;
		gameObjectGravityZ = tempg.z / 9.8;
	}
	else
	{
		isGravityOn = false;
	}
	showDot();


}

void GUIManager::modifyCurrentEntity()
{

	
	

	if (eGameObject != nullptr)
	{

		//TRANSLATION
		if (gameObjectPosOldX != gameObjectPosX)
		{
			eGameObject->translation = glm::translate(eGameObject->translation, glm::vec3(-gameObjectPosOldX, 0, 0));
			eGameObject->transform = eGameObject->translation * eGameObject->rotation * eGameObject->scale;
			eGameObject->translation = glm::translate(eGameObject->translation, glm::vec3(gameObjectPosX, 0, 0));
			eGameObject->transform = eGameObject->translation * eGameObject->rotation * eGameObject->scale;
			gameObjectPosOldX = gameObjectPosX;
		}

		if (gameObjectPosOldY != gameObjectPosY)
		{
			eGameObject->translation = glm::translate(eGameObject->translation, glm::vec3(0, -gameObjectPosOldY, 0));
			eGameObject->transform = eGameObject->translation * eGameObject->rotation * eGameObject->scale;
			eGameObject->translation = glm::translate(eGameObject->translation, glm::vec3(0, gameObjectPosY, 0));
			eGameObject->transform = eGameObject->translation * eGameObject->rotation * eGameObject->scale;
			gameObjectPosOldY = gameObjectPosY;
		}

		if (gameObjectPosOldZ != gameObjectPosZ)
		{
			eGameObject->translation = glm::translate(eGameObject->translation, glm::vec3(0, 0, -gameObjectPosOldZ));
			eGameObject->transform = eGameObject->translation * eGameObject->rotation * eGameObject->scale;
			eGameObject->translation = glm::translate(eGameObject->translation, glm::vec3(0, 0, gameObjectPosZ));
			eGameObject->transform = eGameObject->translation * eGameObject->rotation * eGameObject->scale;
			gameObjectPosOldZ = gameObjectPosZ;
		}


		//ROTATION
		glm::quat tmpQuat(0, 0, 0, 1);
		vec3 eulerAngleTemp;
		eulerAngleTemp = glm::vec3(radians(gameObjectRotX), radians(gameObjectRotY), radians(gameObjectRotZ));
		tmpQuat = quat(eulerAngleTemp);
		eGameObject->rotation = glm::toMat4(tmpQuat);
		eGameObject->transform = eGameObject->translation * eGameObject->rotation * eGameObject->scale;

		//SCALE
		eGameObject->scale = glm::scale(eGameObject->scale, glm::vec3(gameObjectScaleX / gameObjectScaleOldX, gameObjectScaleY / gameObjectScaleOldY, gameObjectScaleZ / gameObjectScaleOldZ));
		eGameObject->transform = eGameObject->translation * eGameObject->rotation * eGameObject->scale;
		gameObjectScaleOldX = gameObjectScaleX;
		gameObjectScaleOldY = gameObjectScaleY;
		gameObjectScaleOldZ = gameObjectScaleZ;


		//RENDERING
		if (rGameObject != nullptr)
		{
			if (isRenderComponentOn)
			{
				rGameObject->enable();
			}
			else if (!isRenderComponentOn)
			{
				rGameObject->disable();
			}
		}

		//PHYSICS
		if (pGameObject != nullptr)
		{
			if (isPhysicsComponentOn)
			{
				pGameObject->enable();

			}
			else if (!isPhysicsComponentOn)
			{
				pGameObject->disable();
			}
		}



		if (pGameObject != nullptr)
		{

			//ISSTATIC
			//MIGHT BE ACTING WEIRD?
			pGameObject->setStatic(isGameObjectStatic);
			eGameObject->setStatic(isGameObjectStatic);
			
			//CHANGE FORCE
			pGameObject->setMomentum(glm::vec3(gameObjectForceX, gameObjectForceY, gameObjectForceZ));
			//TORQUE
			pGameObject->setAngularMomentum(glm::vec3(gameObjectTorqueX, gameObjectTorqueY, gameObjectTorqueZ));

		
		


			//MASS AND MOMENTOFINERTIA
			pGameObject->setMomentOfInertia(PhysicsBuilder::getMomentOfInertia(eGameObject->getMeshID(), eGameObject->scale, gameObjectMass));
			pGameObject->setMass(gameObjectMass);


			//BOUNCINESS
			pGameObject->setCoeffOfRestitution(gameObjectBounciness);


			//eGameObject->addComponent(pGameObject);

			PhysicsEngine::getInstance()->addComponent(pGameObject);

		}


		//GRAVITY
	
		if (!isGravityOn)
		{
			PhysicsEngine::getInstance()->setGravity(glm::vec3(1, 1, 1), 0);
		}
		else if (isGravityOn)
		{
			PhysicsEngine::getInstance()->setGravity(glm::vec3(gameObjectGravityX / 9.8, gameObjectGravityY / 9.8, gameObjectGravityZ / 9.8), 9.8);
		}

		//COLLISION
		if (cGameObject != nullptr)
		{
			if (isCollisionComponentOn)
			{
				cGameObject->enable();

			}
			else if (!isCollisionComponentOn)
			{
				cGameObject->disable();
			}
			cGameObject->updateBoundingShapes();
		}
		

	}
	
	showDot();

}

void GUIManager::selectedGameObject()
{
	if (iteratorGameObject >= EntityManager::getInstance()->getEntities()->size())
	{
		iteratorGameObject = 0;
	}
	eGameObject = EntityManager::getInstance()->getEntities()->at(iteratorGameObject);
	rGameObject = static_cast<RenderComponent*>(eGameObject->getComponent(RENDER_COMPONENT));
	pGameObject = static_cast<PhysicsComponent*>(eGameObject->getComponent(PHYSICS_COMPONENT));
	cGameObject = static_cast<CollisionComponent*>(eGameObject->getComponent(COLLISION_COMPONENT));

	
}

void GUIManager::instantiateDot()
{
	eGameObjectC = new Entity();
	eGameObjectC->setShape(3);
	eGameObjectC->setStatic(false);

	rGameObjectC = new RenderComponent();

	rGameObjectC->setColor(glm::vec3(-1, -1, -1));
	rGameObjectC->disable();
	rGameObjectC->setMeshID(3);
	RenderEngine::getInstance()->addComponent(rGameObjectC);
	eGameObjectC->addComponent(rGameObjectC);
	
}

void GUIManager::showDot()
{
	
	mat4 trans = eGameObject->translation;
	//trans[3][1]
	trans[3][1] += 2* eGameObject->scale[1][1];
	cout << trans[3][1];
	eGameObjectC->transform = trans * eGameObject->scale;
}

GUIManager * GUIManager::getInstance()
{
	return instance;
}


