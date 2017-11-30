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





//TESTING GUI
#include <nanogui/nanogui.h>
#include <iostream>
#include <Eigen/Core>

using namespace nanogui;



//CONSIDER PUTTING THIS AS A STRUCT FOR A NEW GAMEOBJECT
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

Color colval(0.5f, 0.5f, 0.7f, 1.f);

Screen *screen = nullptr;

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






/**
This class right now just sets thigns up, ideally by the end of this it would be a list of settings and nothing else
*/
// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

const int framerate = 120;

WindowManager* windowManager;
Camera* camera;

float modelScale = 1.0f;

int main()
{

#pragma region initRegion
	TimeSystem::begin();

	windowManager = new WindowManager(SCR_WIDTH, SCR_HEIGHT);
	windowManager->setFramerate(framerate);

	Shader shaderProg("ShaderSources/vert.vs", "ShaderSources/frag.fs");

	camera = new Camera(windowManager);
	camera->translate(glm::vec3(0, 0, -100));
	EntityManager::Initialize();
	InputManager::setWindow(windowManager);
	InputManager::setCamera(camera);
	InputManager::initialize();

	

	RenderEngine::Initialize();
	RenderEngine::getInstance()->setShader(&shaderProg);

	PhysicsEngine::Initialize();
	PhysicsEngine::getInstance()->setGravity(glm::vec3(0, -1, 0),0);

	CollisionEngine::Initialize();

	SceneLoading::Initialize();

	vector<std::string> faces=
	{
			"Skybox/TropicalSunnyDayRight2048.png",
			"Skybox/TropicalSunnyDayLeft2048.png",
			"Skybox/TropicalSunnyDayUp2048.png",
			"Skybox/TropicalSunnyDayDown2048.png",
			"Skybox/TropicalSunnyDayBack2048.png",
			"Skybox/TropicalSunnyDayFront2048.png"
	};
	unsigned int cubemapTexture = loadSkybox(faces);
	Mesh::cubemapTexture = cubemapTexture;

#pragma endregion




#pragma region GUITEST


	screen = new Screen();
	screen->initialize(windowManager->getWindow(), true);



	//MAIN MENU WINDOW
	mainMenuWindow = new Window(screen, "Main Menu");
	mainMenuWindow->center();
	mainMenuWindow->setLayout(new GroupLayout());
	Button *bMainOpen = new Button(mainMenuWindow, "Open Existing Scene", ENTYPO_ICON_ARCHIVE);
	bMainOpen->setCallback([] {
		string fileNameExt = file_dialog({ { "scene", "Scene Engine Parser File" } }, false);
		SceneLoading::getInstance()->loadScene(fileNameExt);
		//CHANGE ME
		isNewScene = true;

		isOnEdit = true;
	});
	Button *bMainNew = new Button(mainMenuWindow, "Create New Scene", ENTYPO_ICON_BRUSH);
	bMainNew->setCallback([] {isNewScene = true;	 isOnEdit = true;	});



	//EDITOR WINDOW
	engineEditorWindow = new Window(screen, "Engine Editor");
	engineEditorWindow->setPosition((Eigen::Vector2i(10, 10)));
	engineEditorWindow->setLayout(new GroupLayout());
	engineEditorWindow->setTooltip("Allows user to edit scene, play out the scene and save the scene");
	Button *bEditorPlay = new Button(engineEditorWindow, "Play", ENTYPO_ICON_CONTROLLER_PLAY);
	bEditorPlay->setFlags(Button::RadioButton);
	bEditorPlay->setCallback([] {isOnEdit = false; });
	Button *bEditorEdit = new Button(engineEditorWindow, "Edit", ENTYPO_ICON_EDIT);
	bEditorEdit->setFlags(Button::RadioButton);
	bEditorEdit->setPushed(true);
	bEditorEdit->setCallback([] {isOnEdit = true; });
	Button *bEditorOpen = new Button(engineEditorWindow, "Open Another Scene", ENTYPO_ICON_ARCHIVE);
	bEditorOpen->setCallback([] {
		string fileNameExt = file_dialog({ { "scene", "Scene Engine Parser File" } }, false);
		SceneLoading::getInstance()->loadScene(fileNameExt);
		//CHANGE ME
		isNewScene = true;

		isOnEdit = true;
	});
	Button *bEditorSave = new Button(engineEditorWindow, "Save", ENTYPO_ICON_SAVE);
	//THIS RETURNS THE FILE DESTINATION OF WHERE I DECIDED TO PUT THE FILE
	bEditorSave->setCallback([&] {  cout << "File dialog result: " << file_dialog(
	{ { "scene", "Scene Engine Parser File" } }, true) << endl;

	//IF FILE SAVING WAS SUCCESFUL
	MessageDialog *dlg = new MessageDialog(screen, MessageDialog::Type::Information, "File", "Your scene has been successfully saved.");


	});

	engineEditorWindow->setVisible(false);





	//INSTANTIATION WINDOW
	gameObjectInstantiationWindow = new Window(screen, "GameObject Instantiation");
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
		new ComboBox(gameObjectInstantiationWindow, { "Cube", "Sphere", "Cylinder" });
	//SET UP CALL BACK TO ASSOCIATE SHAPES WITH INSTANTIATION
	cobo->setSide(Popup::Side::Left);
	cobo->setFontSize(16);
	cobo->setFixedSize(Vector2i(100, 20));

	new Label(gameObjectInstantiationWindow, "GameObject Color ", "sans");
	auto cp = new ColorPicker(gameObjectInstantiationWindow, { 255, 120, 0, 255 });
	cp->setFixedSize({ 100, 30 });
	cp->setSide(Popup::Side::Left);
	cp->setFinalCallback([](const Color &c) {
		std::cout << "ColorPicker Final Callback: ["
			<< c.r() << ", "
			<< c.g() << ", "
			<< c.b() << ", "
			<< c.w() << "]" << std::endl;
	});
	//SET UP CALL BACK TO ASSOCIATE COLOR WITH INSTANTIATION
	new Label(gameObjectInstantiationWindow, "Instantiate GameObject ", "sans-bold");
	Button *b = gameObjectInstantiationWindow->add<Button>("Instantiate", ENTYPO_ICON_ROCKET);
	b->setBackgroundColor(Color(0, 100, 255, 25));
	b->setCallback([] { cout << "pushed!" << endl; });
	b->setTooltip("This creates a gameobject with the specified shape and color specified.");

	gameObjectInstantiationWindow->setVisible(false);






	//INSPECTOR WINDOW
	gameObjectInspectorWindow = new Window(screen, "GameObject Inspector");
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
	CheckBox *cb = new CheckBox(gameObjectInspectorWindow, "Render Component");
	cb->setFontSize(16);
	cb->setChecked(true);
	cb->setTooltip("Turn on and off the render component");
	//HOW TO GET THE STATE OF THE CHECK
	//cb->setCallback([&](bool state)
	//{
	//	mergeFaces = state;
	//	m_pQBTFile->SetMergeFaces(mergeFaces);
	//	m_pQBTFile->RecreateStaticBuffers();
	//});
	PopupButton *popupBtnTranslation = new PopupButton(gameObjectInspectorWindow, "Transform", ENTYPO_ICON_EXPORT);
	Popup *popupT = popupBtnTranslation->popup();
	popupT->setLayout(new GridLayout(Orientation::Horizontal, 6,
		Alignment::Middle, 15, 6));
	new Label(popupT, " Translate X");
	popupT->setTooltip("Change the Position, Rotation and Scale of the current GameObject.");
	auto intBox = new FloatBox<float>(popupT);
	intBox->setEditable(true);
	intBox->setFixedSize(Vector2i(100, 20));
	intBox->setValue(0);
	intBox->setDefaultValue("0");
	intBox->setFontSize(16);
	intBox->setSpinnable(true);
	intBox->setValueIncrement(.5);
	new Label(popupT, "Translate Y");
	auto intBox2 = new FloatBox<float>(popupT);
	intBox2->setEditable(true);
	intBox2->setFixedSize(Vector2i(100, 20));
	intBox2->setValue(0);
	intBox2->setDefaultValue("0");
	intBox2->setFontSize(16);
	intBox2->setSpinnable(true);
	intBox2->setValueIncrement(.5);
	new Label(popupT, "Translate Z");
	auto intBox3 = new FloatBox<float>(popupT);
	intBox3->setEditable(true);
	intBox3->setFixedSize(Vector2i(100, 20));
	intBox3->setValue(0);
	intBox3->setDefaultValue("0");
	intBox3->setFontSize(16);
	intBox3->setSpinnable(true);
	intBox3->setValueIncrement(.5);


	new Label(popupT, " Rotate X");
	auto intBox4 = new FloatBox<float>(popupT);
	intBox4->setEditable(true);
	intBox4->setFixedSize(Vector2i(100, 20));
	intBox4->setValue(0);
	intBox4->setDefaultValue("0");
	intBox4->setFontSize(16);
	intBox4->setSpinnable(true);
	intBox4->setValueIncrement(1);
	new Label(popupT, "Rotate Y");
	auto intBox5 = new FloatBox<float>(popupT);
	intBox5->setEditable(true);
	intBox5->setFixedSize(Vector2i(100, 20));
	intBox5->setValue(0);
	intBox5->setDefaultValue("0");
	intBox5->setFontSize(16);
	intBox5->setSpinnable(true);
	intBox5->setValueIncrement(1);
	new Label(popupT, "Rotate Z");
	auto intBox6 = new FloatBox<float>(popupT);
	intBox6->setEditable(true);
	intBox6->setFixedSize(Vector2i(100, 20));
	intBox6->setValue(0);
	intBox6->setDefaultValue("0");
	intBox6->setFontSize(16);
	intBox6->setSpinnable(true);
	intBox6->setValueIncrement(1);

	new Label(popupT, "Scale X");
	auto intBox7 = new FloatBox<float>(popupT);
	intBox7->setEditable(true);
	intBox7->setFixedSize(Vector2i(100, 20));
	intBox7->setValue(0);
	intBox7->setDefaultValue("0");
	intBox7->setFontSize(16);
	intBox7->setSpinnable(true);
	intBox7->setMinValue(0);
	intBox7->setFormat("[0-9]*\\.?[0-9]+");
	intBox7->setValueIncrement(.1);
	new Label(popupT, "Scale Y");
	auto intBox8 = new FloatBox<float>(popupT);
	intBox8->setEditable(true);
	intBox8->setFixedSize(Vector2i(100, 20));
	intBox8->setValue(0);
	intBox8->setDefaultValue("0");
	intBox8->setFontSize(16);
	intBox8->setSpinnable(true);
	intBox8->setMinValue(0);
	intBox8->setFormat("[0-9]*\\.?[0-9]+");
	intBox8->setValueIncrement(.1);
	new Label(popupT, "Scale Z");
	auto intBox9 = new FloatBox<float>(popupT);
	intBox9->setEditable(true);
	intBox9->setFixedSize(Vector2i(100, 20));
	intBox9->setValue(0);
	intBox9->setDefaultValue("0");
	intBox9->setFontSize(16);
	intBox9->setSpinnable(true);
	intBox9->setMinValue(0);
	intBox9->setFormat("[0-9]*\\.?[0-9]+");
	intBox9->setValueIncrement(.1);




	new Label(gameObjectInspectorWindow, "Physics Properties", "sans");
	CheckBox *cb2 = new CheckBox(gameObjectInspectorWindow, "Physics Component");
	cb2->setFontSize(16);
	cb2->setChecked(true);
	cb2->setTooltip("Turn on and off the physics component.");
	//HOW TO GET THE STATE OF THE CHECK
	//HOW TO GET THE STATE OF THE CHECK
	//cb->setCallback([&](bool state)
	//{
	//	mergeFaces = state;
	//	m_pQBTFile->SetMergeFaces(mergeFaces);
	//	m_pQBTFile->RecreateStaticBuffers();
	//});
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

	gameObjectInspectorWindow->setVisible(false);








	screen->performLayout();



	glfwSetCursorPosCallback(windowManager->getWindow(),
		[](GLFWwindow *, double x, double y) {
		screen->cursorPosCallbackEvent(x, y);
	}
	);

	glfwSetMouseButtonCallback(windowManager->getWindow(),
		[](GLFWwindow *, int button, int action, int modifiers) {
		screen->mouseButtonCallbackEvent(button, action, modifiers);
	}
	);

	glfwSetKeyCallback(windowManager->getWindow(),
		[](GLFWwindow *, int key, int scancode, int action, int mods) {
		screen->keyCallbackEvent(key, scancode, action, mods);
	}
	);

	glfwSetCharCallback(windowManager->getWindow(),
		[](GLFWwindow *, unsigned int codepoint) {
		screen->charCallbackEvent(codepoint);
	}
	);

	glfwSetDropCallback(windowManager->getWindow(),
		[](GLFWwindow *, int count, const char **filenames) {
		screen->dropCallbackEvent(count, filenames);
	}
	);

	glfwSetScrollCallback(windowManager->getWindow(),
		[](GLFWwindow *, double x, double y) {
		screen->scrollCallbackEvent(x, y);
	}
	);

	glfwSetFramebufferSizeCallback(windowManager->getWindow(),
		[](GLFWwindow *, int width, int height) {
		screen->resizeCallbackEvent(width, height);
	}
	);

#pragma endregion



	int numX = 1;
	int numY = 1;
	int numZ = 1;

	glCullFace(GL_BACK);

	float disp = 1.0f;

	float fieldSize = 25.f;
	TimeSystem::resetTime();
	PhysicsEngine::getInstance()->disable();
	SceneLoading::getInstance()->loadScene("bowl");
	//std::cout << "test1 "<< camera->skybox->getID() << std::endl;

	glm::mat4 rotation(1.0f), projection;

	GLuint projLoc = glGetUniformLocation(shaderProg.ID, "projection");
	GLuint viewLoc = glGetUniformLocation(shaderProg.ID, "view");
	GLuint modelLoc = glGetUniformLocation(shaderProg.ID, "model");
	Shader::isRenderSkyboxLoc = glGetUniformLocation(shaderProg.ID, "isRenderSkybox");
	projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
	shaderProg.use();

	shaderProg.setMat4(projLoc, projection);
	shaderProg.setVec3("lightDirection", glm::vec3(-1.4, -1.5, -2));
	shaderProg.setVec3("ambientLight", glm::vec3(1.0f, 1.0f, 1.0f));
	float cosT = 0, sinT = 0;
	TimeSystem::resetTime();
	//RenderComponent* r = (RenderComponent*)InputManager::Entities->at(1)->getComponent(RENDER_COMPONENT);
	//RenderComponent* r2 = (RenderComponent*)InputManager::Entities->at(2)->getComponent(RENDER_COMPONENT);

	//r->setColor(glm::vec3(1, 0, 0));
	//r2->setColor(glm::vec3(0, 0, 1));

	//RenderEngine::getInstance()->updateColors();

#pragma region mainLoop
	while (windowManager->windowHasClosed())
	{
		
		float cosT = cosf(glfwGetTime()), sinT = sinf(glfwGetTime());
		glm::mat4 view = camera->GetViewMatrix();
		shaderProg.setMat4(viewLoc, view);
		
		TimeSystem::update();				

		PhysicsEngine::getInstance()->step();
		

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


	
	//MOVE ME
		if (TimeSystem::frameCheck() >= 1.0f / 60.0f) {
			glEnable(GL_DEPTH_TEST);
			InputManager::processInput(); // Not sure if this should be here. Might aswell be inside the physical step tick.
			windowManager->clearWindow();
			shaderProg.use();
			windowManager->drawMeshes();
			screen->drawContents();
			screen->drawWidgets();
			windowManager->swapBuffer();
			glDisable(GL_DEPTH_TEST);
			TimeSystem::frameStep();
		}
	//	windowManager->frameTick();
		
		//DRAWING GUI


		
		//THIS STOPS THE CAMERA FROM MOVING
	

		//windowManager->clearWindow();
	



	}

#pragma endregion

	glfwTerminate();
	delete(windowManager);
	return 0;
}





