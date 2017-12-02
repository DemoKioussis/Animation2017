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
		string fileNameExt = file_dialog({ { "scene", "Scene Engine Parser File" } }, false);
		SceneLoading::getInstance()->loadScene(fileNameExt);


		if (fileNameExt != "")
		{
			isNewScene = true;

			isOnEdit = true;
		}


	});
	Button *bMainNew = new Button(mainMenuWindow, "Create New Scene", ENTYPO_ICON_BRUSH);
	bMainNew->setCallback([&] {isNewScene = true;	 isOnEdit = true;	});


}



void GUIManager::createEditorWindow()
{
	engineEditorWindow = new Window(PhysicsSimulation::getInstance()->getScreen(), "Engine Editor");
	engineEditorWindow->setPosition((Eigen::Vector2i(10, 10)));
	engineEditorWindow->setLayout(new GroupLayout());
	engineEditorWindow->setTooltip("Allows user to edit scene, play out the scene and save the scene");
	Button *bEditorPlay = new Button(engineEditorWindow, "Play", ENTYPO_ICON_CONTROLLER_PLAY);
	bEditorPlay->setFlags(Button::RadioButton);
	bEditorPlay->setCallback([&] {isOnEdit = false; });
	Button *bEditorEdit = new Button(engineEditorWindow, "Edit", ENTYPO_ICON_EDIT);
	bEditorEdit->setFlags(Button::RadioButton);
	bEditorEdit->setPushed(true);
	bEditorEdit->setCallback([&] {isOnEdit = true; });
	Button *bEditorOpen = new Button(engineEditorWindow, "Open Another Scene", ENTYPO_ICON_ARCHIVE);
	bEditorOpen->setCallback([&] {
		string fileNameExt = file_dialog({ { "scene", "Scene Engine Parser File" } }, false);
		SceneLoading::getInstance()->loadScene(fileNameExt);
		//CHANGE ME

		if (fileNameExt != "")
		{
			isNewScene = true;

			isOnEdit = true;
		}

	});
	Button *bEditorSave = new Button(engineEditorWindow, "Save", ENTYPO_ICON_SAVE);
	//THIS RETURNS THE FILE DESTINATION OF WHERE I DECIDED TO PUT THE FILE
	bEditorSave->setCallback([&] {  cout << "File dialog result: " << file_dialog(
	{ { "scene", "Scene Engine Parser File" } }, true) << endl;

	//IF FILE SAVING WAS SUCCESFUL
	MessageDialog *dlg = new MessageDialog(PhysicsSimulation::getScreen(), MessageDialog::Type::Information, "File", "Your scene has been successfully saved.");


	});

	engineEditorWindow->setVisible(false);
}

void GUIManager::createInstantiationWindow()
{
	gameObjectInstantiationWindow = new Window(PhysicsSimulation::getScreen(), "GameObject Instantiation");
	gameObjectInstantiationWindow->setPosition((Eigen::Vector2i(1400, 10)));
	gameObjectInstantiationWindow->setLayout(new BoxLayout(Orientation::Vertical,
		Alignment::Middle, 15, 6));
	new Label(gameObjectInstantiationWindow, "GameObject Name", "sans");
	TextBox *textBox2 = new TextBox(gameObjectInstantiationWindow);
	textBox2->setEditable(true);
	//MAKE A REGEX TO ONLY ALLOW LETTERS AND NUMBERS
	//textBox->setFormat("^[0-9][a-z][A-Z]");
	textBox2->setFixedSize(Vector2i(120, 20));
	textBox2->setFontSize(16);
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
	scaleBoxX->setDefaultValue("0");
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
	scaleBoxY->setDefaultValue("0");
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
	scaleBoxZ->setDefaultValue("0");
	scaleBoxZ->setFontSize(16);
	scaleBoxZ->setSpinnable(true);
	scaleBoxZ->setMinValue(0.1);
	scaleBoxZ->setFormat("[0-9]*\\.?[0-9]+");
	scaleBoxZ->setValueIncrement(.1);
	scaleBoxZ->setCallback([&](float z) {  if (z <= 0.1)z = .1; gameObjectScaleZ = z; modifyCurrentEntity(); });

}


void GUIManager::createInspectorWindow()
{
	gameObjectInspectorWindow = new Window(PhysicsSimulation::getScreen(), "GameObject Inspector");
	gameObjectInspectorWindow->setPosition((Eigen::Vector2i(10, 300)));
	gameObjectInspectorWindow->setLayout(new GroupLayout());
	new Label(gameObjectInspectorWindow, "GameObject Name", "sans-bold");
	TextBox *textBox = new TextBox(gameObjectInspectorWindow);
	textBox->setEditable(true);
	//MAKE A REGEX TO ONLY ALLOW LETTERS AND NUMBERS
	//textBox->setFormat("^[0-9][a-z][A-Z]");
	textBox->setFixedSize(Vector2i(120, 20));
	textBox->setFontSize(16);

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
	cb->setCallback([&](bool state)
	{
		isPhysicsComponentOn = true;
		modifyCurrentEntity();
	});
	PopupButton *popupBtnForce = new PopupButton(gameObjectInspectorWindow, "Force", ENTYPO_ICON_EXPORT);
	Popup *popupF = popupBtnForce->popup();
	popupF->setLayout(new GridLayout(Orientation::Horizontal, 6,
		Alignment::Middle, 15, 6));
	popupF->setTooltip("Change the Force of the current GameObject.");
	new Label(popupF, " Apply Force X");
	auto intBox10 = new FloatBox<float>(popupF);
	intBox10->setEditable(true);
	intBox10->setFixedSize(Vector2i(100, 20));
	intBox10->setValue(0);
	intBox10->setDefaultValue("0");
	intBox10->setFontSize(16);
	intBox10->setSpinnable(true);
	intBox10->setValueIncrement(.5);
	new Label(popupF, " Apply Force  Y");
	auto intBox11 = new FloatBox<float>(popupF);
	intBox11->setEditable(true);
	intBox11->setFixedSize(Vector2i(100, 20));
	intBox11->setValue(0);
	intBox11->setDefaultValue("0");
	intBox11->setFontSize(16);
	intBox11->setSpinnable(true);
	intBox11->setValueIncrement(.5);
	new Label(popupF, " Apply Force  Z");
	auto intBox12 = new FloatBox<float>(popupF);
	intBox12->setEditable(true);
	intBox12->setFixedSize(Vector2i(100, 20));
	intBox12->setValue(0);
	intBox12->setDefaultValue("0");
	intBox12->setFontSize(16);
	intBox12->setSpinnable(true);
	intBox12->setValueIncrement(.5);
	new Label(popupF, "Mass");
	auto intBox13 = new FloatBox<float>(popupF);
	intBox13->setEditable(true);
	intBox13->setFixedSize(Vector2i(100, 20));
	intBox13->setValue(0);
	intBox13->setDefaultValue("0");
	intBox13->setFontSize(16);
	intBox13->setSpinnable(true);
	intBox13->setMinValue(0);
	intBox13->setFormat("[0-9]*\\.?[0-9]+");
	intBox13->setValueIncrement(1);
	new Label(popupF, "Bounciness");
	auto intBox14 = new FloatBox<float>(popupF);
	intBox14->setEditable(true);
	intBox14->setFixedSize(Vector2i(100, 20));
	intBox14->setValue(0);
	intBox14->setDefaultValue("0");
	intBox14->setFontSize(16);
	intBox14->setSpinnable(true);
	intBox14->setMinValue(0);
	intBox14->setFormat("[0-9]*\\.?[0-9]+");
	intBox14->setValueIncrement(.1);
	CheckBox *cbs = new CheckBox(popupF, "Static");
	cbs->setFontSize(16);
	cbs->setChecked(false);
	cbs->setTooltip("Allow the GameObject to stay in place");



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
	b->setCallback([&] {updateButtons(); });
	b->setTooltip("Iterates to the next gameobject in the scene.");

	gameObjectInspectorWindow->setVisible(false);
	PhysicsSimulation::getScreen()->performLayout();

}




GUIManager::GUIManager()
{

	createMainMenuWindow();
	createEditorWindow();
	createInstantiationWindow();
	createInspectorWindow();

}






void GUIManager::interactWithGUI()
{

	//MOVE ME
	if (isOnEdit)
	{
		gameObjectInstantiationWindow->setVisible(true);
		gameObjectInspectorWindow->setVisible(true);
		PhysicsEngine::getInstance()->disable();
	}
	else if (!isOnEdit)
	{
		gameObjectInstantiationWindow->setVisible(false);
		gameObjectInspectorWindow->setVisible(false);
		PhysicsEngine::getInstance()->enable();
	}

	if (isNewScene)
	{
		engineEditorWindow->setVisible(true);
		mainMenuWindow->setVisible(false);
	}



}

void GUIManager::instantiateGameObject(glm::vec3 col, int shape)
{

	eGameObject = new Entity(true);
	rGameObject = new RenderComponent();
	rGameObject->setMeshID(shape);
	rGameObject->setColor(col);
	rGameObject->enable();
	RenderEngine::getInstance()->addComponent(rGameObject);
	eGameObject->addComponent(rGameObject);



	pGameObject = new PhysicsComponent();

	pGameObject->enable();
	pGameObject->setMomentOfInertia(PhysicsBuilder::getMomentOfInertia(0, glm::mat4(), mass));
	pGameObject->setMass(mass);
	pGameObject->setStatic(false);
	PhysicsEngine::getInstance()->addAttractor(pGameObject);
	PhysicsEngine::getInstance()->addComponent(pGameObject);
	//	p->setCoeffOfRestitution(stof(sVec[5]));
	eGameObject->addComponent(pGameObject);

	cGameObject = new CollisionComponent();
	cGameObject->enable();
	cGameObject->setMeshID(shape);
	CollisionEngine::getInstance()->addComponent(cGameObject);
	eGameObject->addComponent(cGameObject);
	CollisionEngine::getInstance()->updateAllBoundingBoxes();
	//PhysicsEngine::getInstance()->addForce(p, force, glm::vec3());


	//Add To EntityManager;
	EntityManager::getInstance()->add(eGameObject);

	updateButtons();
}


void GUIManager::updateButtons()
{
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
}

void GUIManager::updateCurrentEntity()
{
	isRenderComponentOn = rGameObject->isEnabled();
	isPhysicsComponentOn = pGameObject->isEnabled();
	isCollisionComponentOn = cGameObject->isEnabled();


	//CHANGE THESE TO THE CURENT GAMEOBJECT;

	gameObjectPosX = eGameObject->translation[3][0];
	gameObjectPosY = eGameObject->translation[3][1];
	gameObjectPosZ = eGameObject->translation[3][2];

	gameObjectPosOldX = 0;
	gameObjectPosOldY = 0;
	gameObjectPosOldZ = 0;

	glm::quat tmpQuat = glm::toQuat(eGameObject->rotation);
	glm::vec3 tmp = glm::eulerAngles(tmpQuat);
	gameObjectRotX = degrees(tmp.x);
	gameObjectRotY = degrees(-tmp.y);
	gameObjectRotZ = degrees(tmp.z);



	gameObjectScaleX = eGameObject->scale[0][0];
	gameObjectScaleY = eGameObject->scale[1][1];
	gameObjectScaleZ = eGameObject->scale[2][2];


}

void GUIManager::modifyCurrentEntity()
{

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
	}

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

		//ROTATION
		eGameObject->translation = glm::scale(eGameObject->translation, glm::vec3(gameObjectScaleX / gameObjectScaleOldX, gameObjectScaleY / gameObjectScaleOldY, gameObjectScaleZ / gameObjectScaleOldZ));
		eGameObject->transform = eGameObject->translation * eGameObject->rotation * eGameObject->scale;
		gameObjectScaleOldX = gameObjectScaleX;
		gameObjectScaleOldY = gameObjectScaleY;
		gameObjectScaleOldZ = gameObjectScaleZ;






	}

}

GUIManager * GUIManager::getInstance()
{
	return instance;
}
