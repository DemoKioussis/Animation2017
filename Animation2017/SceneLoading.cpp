#include "SceneLoading.h"
#include "PhysicsBuilder.h"
SceneLoading* SceneLoading::instance=nullptr;

void SceneLoading::Initialize() {
	if (instance)
		return;
	SceneLoading * sceneLoading = new SceneLoading();
	instance = sceneLoading;
	//instance->setRenderCollisionMeshes();
}
 SceneLoading::SceneLoading() {
	cube = new Mesh("cube.obj", MeshType::VERTICES);
	cylinder = new Mesh("cylinder.obj", MeshType::VERTICES);
	ellipsoid = new Mesh("ellipsoid.obj", MeshType::VERTICES);
	sphere = new Mesh("sphere.obj", MeshType::SPHERE);
	sphereLR = new Mesh("sphereLR.obj", MeshType::VERTICES);
	//sphereHD = new Mesh("sphereHD.obj", MeshType::SPHERE);
	RenderEngine::getInstance()->addRenderReference(cube);
	RenderEngine::getInstance()->addRenderReference(cylinder);
	RenderEngine::getInstance()->addRenderReference(ellipsoid);
	RenderEngine::getInstance()->addRenderReference(sphere);
	RenderEngine::getInstance()->addRenderReference(sphereLR);
	//RenderEngine::getInstance()->addRenderReference(sphereHD);
	CollisionEngine::getInstance()->addMesh(cube);
	CollisionEngine::getInstance()->addMesh(cylinder);
	CollisionEngine::getInstance()->addMesh(ellipsoid);
	CollisionEngine::getInstance()->addMesh(sphere);
	CollisionEngine::getInstance()->addMesh(sphereLR);
	//CollisionEngine::getInstance()->addMesh(sphereHD);

	PhysicsEngine::getInstance()->meshes.push_back(cube);
	PhysicsEngine::getInstance()->meshes.push_back(cylinder);
	PhysicsEngine::getInstance()->meshes.push_back(ellipsoid);
	PhysicsEngine::getInstance()->meshes.push_back(sphere);
	PhysicsEngine::getInstance()->meshes.push_back(sphereLR);
	//CollisionEngine::getInstance()->addMesh(sphereHD);
}

void SceneLoading::loadScene(char * sceneName) {

	char pathfile[100];
	strcpy_s(pathfile, "Scenes/");
	strcat_s(pathfile, sceneName);
	strcpy_s(pathfile, pathfile);

	strcat_s(pathfile, ".scene");
	cout << pathfile << endl;
	ifstream scene;
	scene.open(pathfile);
	if (!scene) {
		std::cout << "Unable to open scene: " << pathfile << std::endl;
		return;
	}
	float timeScale = TimeSystem::getTimeScale();
	TimeSystem::setTimeScale(0);
	//float elapsedTime = glfwGetTime();
	InputManager::Entities->clear();
	RenderEngine::Clear();
	PhysicsEngine::Clear();
	CollisionEngine::Clear();

	string line;
	int currentMeshIndex;
	Entity* e = nullptr;
	glm::vec3 force(0,0,0);
	while (getline(scene, line)) {
		splitstring sLine = splitstring(line);
		vector<string> sVec= sLine.split('|');
		if (sVec.size() == 0) {
			continue;
		}
		if (isdigit(sVec[0][0])) {//new entity
			//std::cout << "new ent" << std::endl;
			currentMeshIndex = stoi(sVec[0]); 
			if (e!=nullptr) {//entity loaded all components/add to list
				//std::cout << "pushed" << std::endl;

				InputManager::Entities->push_back(e);
			}
			Entity* etmp = new Entity(stoi(sVec[1]));
			e = etmp;
			continue;
		}
		else {
			//load components
			if (sVec[0] == "position") {//position
				//std::cout << "position" << std::endl;
				e->translation =glm::translate(e->translation, glm::vec3(stof(sVec[1]), stof(sVec[2]), stof(sVec[3])));
			}
			if (sVec[0] == "scale") {//scale
				e->scale = glm::scale(e->scale, glm::vec3(stof(sVec[1]), stof(sVec[2]), stof(sVec[3])));
			}
			if (sVec[0] == "render") {//render
				//std::cout << "render" << std::endl;
				RenderComponent *r = new RenderComponent();
				r->setMeshID(currentMeshIndex);
				e->addComponent(r);
				RenderEngine::getInstance()->addComponent(r);
			}
			if (sVec[0] == "physics") {
				//std::cout << "physics" << std::endl;
				PhysicsComponent* p = new PhysicsComponent();
				float mass = stof(sVec[1]);
				p->setMomentOfInertia(PhysicsBuilder::getMomentOfInertia(currentMeshIndex, glm::mat4(),mass));
				p->setMass(mass);
				e->addComponent(p);
				PhysicsEngine::getInstance()->addComponent(p);
				//std::cout << e->translation[3][0]<< e->translation[3][1]<< e->translation[3][2] << std::endl;
				//system("pause");
				force.x = stof(sVec[2]);
				force.y = stof(sVec[3]);
				force.z = stof(sVec[4]);
				PhysicsEngine::getInstance()->addForce(p, glm::vec3(stof(sVec[2]), stof(sVec[3]), stof(sVec[4])),e->translation[3]);
				//PhysicsEngine::getInstance()->
			}
			if (sVec[0] == "collider") {
				//std::cout << "collider" << std::endl;
				CollisionComponent* c = new CollisionComponent();
				c->setMeshID(currentMeshIndex);
				e->addComponent(c);
				CollisionEngine::getInstance()->addComponent(c);

			}
		}
	}
	if (e != nullptr) {//entity loaded all components/add to list
		//std::cout << "pushed" << std::endl;

		InputManager::Entities->push_back(e);
	}
	
	CollisionEngine::getInstance()->updateAllBoundingBoxes(); // Can only be called after calculating the unique indices

	//glfwSetTime(elapsedTime);
	TimeSystem::setTimeScale(timeScale);
	TimeSystem::resetTime();
}
SceneLoading* SceneLoading::getInstance() {
	return instance;
}


//SceneLoading::~SceneLoading()
//{
//}
